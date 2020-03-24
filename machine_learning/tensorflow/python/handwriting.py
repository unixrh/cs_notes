from tensorflow.examples.tutorials.mnist import input_data
mnist = input_data.read_data_sets("MINIST_data/", one_hot=True)
import pylab
import tensorflow.compat.v1 as tf

tf.disable_eager_execution()

tf.reset_default_graph()
x = tf.placeholder(tf.float32, [None, 784])
y = tf.placeholder(tf.float32, [None, 10])

W = tf.Variable(tf.random_normal([784, 10]))
b = tf.Variable(tf.zeros([10]))

pred = tf.nn.softmax(tf.matmul(x, W) + b)

cost = tf.reduce_mean(-tf.reduce_sum(y*tf.log(pred), reduction_indices=1))

learning_rate = 0.01

optimizer = tf.train.GradientDescentOptimizer(learning_rate).minimize(cost)

training_epochs = 25
batch_size = 100
display_step = 1

saver = tf.train.Saver(max_to_keep=1)
savedir = "model/"
fileprefix = "handwriting.ckpt"

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    for epoch in range(training_epochs):
        avg_cost = 0
        total_batch = int(mnist.train.num_examples/batch_size)
        for i in range(total_batch):
            batch_xs, batch_ys = mnist.train.next_batch(batch_size)
            _, c = sess.run([optimizer, cost], feed_dict={x:batch_xs, y:batch_ys})

            avg_cost += c / total_batch
        if(epoch + 1) % display_step == 0:
            print("Epoch: ", '%04d' % (epoch+1), "cost=", "{:.9f}" . format(avg_cost))
    print("Finished")

    correct_pred = tf.equal(tf.argmax(pred, 1), tf.argmax(y, 1))
    accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))
    print("Accuracy: ", accuracy.eval({x:mnist.test.images, y:mnist.test.labels}))

    save_path = saver.save(sess, savedir+fileprefix)
    print("Model saved in file: %s" % save_path)

print("starting 2nd session...")
with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    saver.restore(sess, savedir+fileprefix)

    correct_pred = tf.equal(tf.argmax(pred, 1), tf.argmax(y, 1))

    accuracy = tf.reduce_mean(tf.cast(correct_pred, tf.float32))
    print("Accuracy: ", accuracy.eval({x:mnist.test.images, y:mnist.test.labels}))

    output = tf.argmax(pred, 1)

    batch_xs, batch_ys = mnist.train.next_batch(2)
    outputval, predv = sess.run([output, pred], feed_dict={x:batch_xs})
    print(outputval, predv, batch_ys)

    im = batch_xs[0]
    im = im.reshape(-1, 28)
    pylab.imshow(im)
    pylab.show()

    im = batch_xs[1]
    im = im.reshape(-1, 28)
    pylab.imshow(im)
    pylab.show()
