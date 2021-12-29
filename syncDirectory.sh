#!/bin/bash
echo "copying..."


cp -r /opt/rtu_t36z1/src/rtu/protocol/ ./

rm -rf protocol/*.o

sync


ls -l


git status


read -p "please input commit message:" message
echo "\n"

echo "commit message is $message" 


git commit -m "$message"

git add .

git push origin master




