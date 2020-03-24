i)mport tensorflow.compat.v1 as tf
tf.disable_v2_behavior()
import numpy as np
import matplotlib.pyplot as plt

plotdata = {"batchsize":[], "loss":[]}
def moving_average(a, w=10):
    if len(a) < w:
        return a[:]
    return [val if idx < w else sum(a[(idx-w):idx]) / w for idx , val in enumerate(a)]

train_X = np.linspace(-1, 1, 100)
train_Y = 2 * train_X + np.random.randn(*train_X.shape) * 0.3
plt.plot(train_X, train_Y, 'ro', label = 'Original data')
plt.legend()
plt.show()

X = tf.placeholder("float")
Y = tf.placeholder("float")

W = tf.Variable(tf.random_normal([1]), name='weight')
b = tf.Variable(tf.zeros([1]), name='bias')
z = tf.multiply(X, W) + b
tf.summary.histogram('z', z)

cost = tf.reduce_mean(tf.square(Y-z))
tf.summary.scalar('loss_function', cost)
learning_rate = 0.01
optimizer = tf.train.GradientDescentOptimizer(learning_rate).minimize(cost)

init = tf.global_variables_initializer()
training_epochs = 20
display_step = 2

saver = tf.train.Saver(max_to_keep=1)
savedir = "model/"
fileprefix = "linearmodel.ckpt"

with tf.Session() as sess:
    sess.run(init)
    merged_summary_op = tf.summary.merge_all()
    summary_writer = tf.summary.FileWriter('log/smry', sess.graph)

    for epoch in range(training_epochs):
        for (x, y) in zip(train_X, train_Y):
            sess.run(optimizer, feed_dict={X: x, Y:y})
        summary_str = sess.run(merged_summary_op, feed_dict={X:x, Y:y})
        summary_writer.add_summary(summary_str, epoch)

        if epoch % display_step == 0:
            loss = sess.run(cost, feed_dict={X:train_X, Y:train_Y})
            print("Epoch:" , epoch+1, "cost=", loss, "W=", sess.run(W), "b=", sess.run(b))
            if not (loss == "NA"):
                plotdata["batchsize"].append(epoch)
                plotdata["loss"].append(loss)
                saver.save(sess, savedir + fileprefix, global_step=epoch)


    print("finished")
    print("cost=", sess.run(cost, feed_dict={X:train_X, Y:train_Y}), "W=", sess.run(W), "b=", sess.run(b))

    plt.plot(train_X, train_Y, 'ro', label='Original data')
    plt.plot(train_X, sess.run(W) * train_X + sess.run(b), label='Fittedline')
    plt.legend()
    plt.show()

    plotdata["avgloss"] = moving_average(plotdata["loss"])
    plt.figure(1)
    plt.subplot(211)
    plt.plot(plotdata["batchsize"], plotdata["avgloss"], 'b--')
    plt.xlabel('Minibatch number')
    plt.ylabel('Loss')
    plt.title('Minibatch run vs. Training loss')
    plt.show()

    print("x=0.2, z=", sess.run(z, feed_dict={X: 0.2}))

load_epoch = 18
with tf.Session() as sess2:
    sess2.run(tf.global_variables_initializer())
    saver.restore(sess2, savedir + fileprefix + "-" + str(load_epoch))
    print("x=0.2, z=", sess2.run(z, feed_dict={X: 0.2}))

