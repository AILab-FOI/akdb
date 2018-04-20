#ubuntu image
FROM ubuntu

#install required files
RUN apt-get update -y
RUN apt-get install git build-essential swig make doxygen python2.7 python-pip dos2unix -y

#installing latex takes some time, comment out the lines 9 and 10 if you do not plan to compile documentation(make doc)
RUN apt-get install texlive -y
RUN apt-get install texlive-latex-extra -y
RUN pip install virtualenv

RUN mkdir -p /usr/akdb
VOLUME ["/usr/akdb"]
WORKDIR /usr/akdb
RUN cd /usr/akdb
