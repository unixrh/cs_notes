# framework
1. model_servers/main.cc  main()
*this is the entry*
- parse the flags from commandline input to server_options
- TFS::main::server.BuildAndStart(const Options& server_options)

2. model_servers/server.{h,cc}  BuildAndStart()
*fill args to options with server_options*
- check_args:
    - grpc_port, model_base_path, model_config_file, model_config_file
- build options.model_server_config:
    - BuildSingleModelConfig(server_options.model_name, server_options.model_base_path);
    - or ParseProtoTextFile
- build options.platform_config_map
    - build session_bundle_config and CreateTensorFlowPlatformConfigMap(session_bundle_config, use_saved_model)
    - or ParseProtoTextFile
- set options.custom_model_config_loader *this is a function pointer*
- set other attrs in options
- build ServerCore instance with options
- start a thread of PeriodFunction type to PollFilesystemAndReloadConfig
    - parse the config
    - ServerCore::ReloadConfig(config): try to refresh the model
- build ModelServiceImpl instance with ServerCore
- build PredictionServiceImpl instance with ServerCore
- build ProfilerService
- init grpc::ServerBuilder
- ServerBuilder.AddListeningPort
- ServerBuilder.RegisterService(model_service prediction_service profiler_service)

# business
