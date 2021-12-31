#!/bin/bash
echo "copying..."


cp -r /opt/rtu_t36z1/src/rtu/protocol/ ./

rm -rf protocol/*.o

sync


ls -l


git status


#read -p "please input commit message:" message
echo "\n"



echo "commit message is $message" 

#git commit -m "$message"

#git add .

#为了保证上传到github的文件可以上传成功，先要将所有的文件缓存到，再使用push一并传上去
#git pull --rebase origin master
#git pull origin master

#push指令上传文件

#git push origin master



