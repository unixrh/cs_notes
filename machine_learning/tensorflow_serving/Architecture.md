# 架构
TFS(TensorFlow Serving)是一个灵活、高性能、为生产环境而设计的机器学习模型服务框架。TFS使用统一的架构及API，使得部署新算法、新实验变得更加简单。对于TF模型，TFS提供开箱即用的封装，TFS也容易扩展服务于其他类型的模型。

## 关键概念
为了便于理解TFS的架构，我们需要先理解以下关键概念：

### Servable
`Servable`是TFS架构的中心，它是服务于客户端请求计算（例如查表或推断）的底层对象。

Servable的大小和粒度很灵活。 一个简单的Servable可以仅包含一个查询表单个分片的内容，也可以包含整个模型的内容，甚至于可以包含一组推断模型。Servable可以是任意的类型和接口，这是出于灵活以及未来的提升考虑的：
- 流式计算结果
- 实验性API
- 异步操作模式

Servable不管理自己的生命周期。

典型的Servable包含以下内容：
- 一个TF模型的SavedModelBundle(tensorflow::Session)
- 一个用于embed或者词汇的查找表

### Servable Versions
在服务实例的生命周期内，TFS可以处理一个Servable的一个或多个版本的服务。这使它具备了实时刷新算法配置、权重以及其他数据的能力。Versions的特性使单个Servable的多个版本可以并行地加载，并且支持渐进式推出和实验。在服务期间，对于某个特定的模型，客户端可以请求最新的版本或者指定版本服务。

### Servable Streams
一个Servable Streams使用一个或多个Servable来代表一个模型。一个机器学习模型可能包含一个或者多个算法（包括权重）、查找和embed表。

对于组合模型，可以通过如下方式表示：
- 多个独立的Servable
- 一个组合Servable

一个Servable也可以对应于模型的一部分。例如，一张很大的查找表可以分片到多个TFS实例中。

### Loaders
Loaders的角色是管理Servable的生命周期。Loader API将通用的基础框架独立出来，与特定的机器学习算法、数据以及产品用例无关。特别地，Loaders还将Servable的loading、unloading的API标准化。

### Sources
Sources是用于查找并提供Servable的插件模块。每个Source提供0个或者多个Servable Stream。对于每个Servable Stream，一个Source为每个可加载的版本提供一个Loader实例。（一个Source实际上连接着许多SourceAdapter，链条的最后一项负责loaders）

TFS中Source的接口可以从任意的存储系统中发现Servable。TFS包含通用的Source实现。例如，Sources可以访问RPC机制或者轮询一个文件系统。

Sources可以维护多个Servable或者Versions之间的状态，这对于使用多个版本的Servable很有用。

### Aspired Versions
Aspired Versions代表一组已经载入并ready的Servable Versions。Sources每次把这组Servable Versions组成Servable Stream。当Source向Manager给出一个新的Aspired Versions列表，这个列表将取代Servable Stream之前的列表。Manager将会unload不在list中的旧版本。

### Manager
Manager处理Servable的完整生命周期，包括：
- 加载Servable
- 使用Servable提供服务
- 卸载Servable

Manager监听Sources并追踪所有版本。它尽量满足Sources的需求，但也可能在一定情形下拒绝加载Aspired Version，比如所需的资源不可用时。Manager同样也可能推迟卸载动作，比如它会等新的版本加载好之后才把旧的卸载掉。这基于版本控制策略的配置。

TFS的Manager提供一个简单、狭隘的接口：`-- GetServableHandle()`，用于给客户端访问Servable实例。

### Core
TFS Core使用标准的TFS API来管理Servable的以下方面：
- 生命周期
- 指标

TFS Core将Servable和Loader视为非透明对象。

## Servable的生命周期

![Servable的生命周期](https://www.tensorflow.org/tfx/serving/images/serving_architecture.svg)

宽泛地说：
1. Sources为Servable Versions创建Loaders。
2. Loaders被当做Aspired Versions送往Manager，这样服务就能加载并服务于客户端。

具体而言：
1. Source插件为特定的版本创建Loader。这个Loader包含了加载Servable所需的所有metadata。
2. Source使用回调来通知Manager关于Aspired Version的信息。
3. Manager基于配置的版本策略来决定下一步的行动，可能是卸载一个就的版本或者加载一个新的版本。
4. 如果Manager确认当前是安全的，它就向Loader提供所需资源并告知Loader加载新版本。
5. 客户端向Manager请求Servable，可能请求最新版本或者某个特定版本。Manager返回该版本Servable的句柄。

举例来说，一个表示权重常更新的TF流程图的Source，权重存储于磁盘文件。
1. Source检测到有新版的模型权重，就会创建一个包含该模型数据指针的Loader。
2. Source通知Dynamic Manager关于Aspired Version的信息。
3. Dynamic Manager基于版本策略决定加载新的模型。
4. Dynamic Manager告知Loader当前系统内存空间足够。Loader就是用心的权重来实例化TF流程图。
5. 客户端请求该模型最新版本的Servable，Dynamic Manager就返回该新版Servable的句柄。

## 扩展性
TFS提供了几个扩展点便于你加入新的功能。

### 版本策略
版本策略指定单个Servable Stream的版本加载和卸载的顺序。

TFS包含了两种最常见用例的版本策略。一是可用性优先策略（避免内存中无Servable，也就是在卸载旧版前加载新版）；另一个是资源优先策略（避免同时有两个版本的Servable在内存中，也就是在加载新版之前先卸载旧版）。对于服务可用性很重要同时资源消耗较小的简单场景，可用性优先策略可以保证在卸载旧版之前加载好新版。对于需要维护在多个实例之间维护多个版本的复杂常场景，资源优先策略占用的资源较少（不需要额外的缓存来加载新版数据）。

### Source
新的Source可以支持新的文件系统、云端数据以及算法后台。TFS提供了通用的方式来简单快速地创建新的模型。例如，TFS包含一个工具，封装了对于一个简单资源的轮询行为。对于特定算法和数据Servable的Loader，Source与之密切相关。

### Loaders
Loaders是用来增加算法和数据后台的扩展点。TF是其中的一种算法后台。举例而言，你可以实现一个新的Loader，用以对新的机器学习模型Servable的加载、提供访问、卸载操作。比如为查找表和额外的算法创建Loaders。

### Batcher
将多个请求合并为单个请求进行批处理，可以显著地降低推断的消耗，尤其是在有诸如GPU的硬件加速支持时。TFS包含批处理插件来让客户端方便地将特定类型的推断请求打包成批处理请求，使得算法可以更高效地处理。