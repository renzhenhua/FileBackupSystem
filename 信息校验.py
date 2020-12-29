# -*- coding：utf-8 -*-

import hashlib
import os



def USE_MD5(test):
    if not isinstance(test, bytes):
        test = bytes(test, 'utf-8')
    m = hashlib.md5()
    m.update(test)
    return m.hexdigest()


if __name__ == '__main__':
    print("请输入文件夹目录，注意该目录下不能有其他文件夹：")
    path = input()  # 文件夹目录
    files = os.listdir(path)  # 得到文件夹下的所有文件名称
    for file in files:  # 遍历文件夹
        if not os.path.isdir(file):  # 判断是否是文件夹，不是文件夹才打开
            f = open(path + "/" + file, encoding='gb18030', errors='ignore')  # 打开文件
            lines = f.read()
            f.close()
            md5_test = USE_MD5(lines)
            f = open("Label.txt", 'a')  # 读取label.txt文件，没有则创建，‘a’表示再次写入时不覆盖之前的内容
            f.write(file + '\t' + md5_test + '\n')  # 输入文件名和计算得到的值
            f.close()

    print("请输入备份文件路径：")
    path = input()  # 备份文件路径
    new_path = "D:/new_document"  # 在这个路径下创建一个文件夹，以供装入解包解压后的文件
    if not os.path.exists(new_path):
        os.makedirs(new_path)
    ######################### 调用 解包、解压把备份后文件解包、解压到新建文件夹中  #######################
    files = os.listdir(new_path)  # 得到文件夹下的所有文件名称
    for file in files:  # 遍历文件夹
        if not os.path.isdir(file):  # 判断是否是文件夹，不是文件夹才打开
            f = open(path + "/" + file, encoding='gb18030', errors='ignore')  # 打开文件
            lines = f.read()
            f.close()
            md5_test = USE_MD5(lines)
            f = open("Label_two.txt", 'a')  # 读取label_two.txt文件，没有则创建，‘a’表示再次写入时不覆盖之前的内容
            f.write(file + '\t' + md5_test + '\n')  # 输入文件名和计算得到的值
            f.close()
    # 逐行比较两个文档中的内容，如果某一行出现了错误则说明该行文件备份出错
    flag = 0
    with open('Label.txt', 'r') as f1:
        with open('Label_two.txt', 'r') as f2:
            for line1,line2 in zip(f1,f2):
                if not line1 == line2:
                    name = line1.split('\t', 1)[0]
                    print(name + "文件备份出错")
                    flag = 1
            if flag == 0:
                print("文件校验完毕")
    os.remove("Label.txt")
    os.remove("Label_two.txt")  # 删除当前目录下的两个文档，防止多次写入
    files = os.listdir("D:/new_document")  # 得到文件夹下的所有文件名称
    for file in files:  # 遍历文件夹
        os.remove("D:/new_document/" + file)  # 删除文件
    os.removedirs("D:/new_document")  # 删除文件夹
