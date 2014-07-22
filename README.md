errifier
========

An error Notifying application that makes error discovery faster

Why errifier
============

errifier was made with the idea to make the error discovery faster. That means, to get notified about the error as soon as it occurs. Without actually constantly looking at the logs, waiting for the error to occur, or to wait for someone else to tell you that "this particular thing isn't working", in which case again you will look at the logs to know what actually happened.

errifier makes your job easy, by looking for errors and then by notifying you about it, when it occurs; and in the mean time you can actually work on what ever useful thing you were working on.


Let the error occur when it has to occur
---------------------------------------

How To Use It?
==============

Following a scenario that i believe, where it can be useful, however you are free to think of any other scenario that you think of

So, Suppose you are a server guy, who has to manage the server. That means you know, the importance that error log plays in your work right? So traditionally how you were managing logs: You open log and see whats the timestamp of last error, Or you would open your mail for logs that contain error and what not. So, did you see, how much time is wasted on opening those logs and then locating errors.

Now errifier does this job for you, the simple, fast, easy to use and lightweight application will notify you about the error as soon as it occurs. You even get to choose when to monitor that particular log and when not, all it requires is a click. 

Supported Platforms
===================

Only Linux(Tested on Ubuntu, should work on others too) as of now


Dependency
==========
* QT
* libnotify
* inotify

Installation
============

First do remember to install QT, libnotify and inotify in the system, then run these command in order

* qmake -o "Makefile" errifier.pro
* make

Double click errifier(application generated to run the appliation)

Contact Me
==========

E-Mail: ayush.choubey@gmail.com

