# Win2016LPE
Windows10 &amp; Windows Server 2016 LPE Exploit</br>
(利用 schedsvc!SchRpcSetSecurity() 漏洞)</br>
# 目录说明
bin-x86 为支持Win10 x86系统的Bin</br>
bin-x64 为支持Win10 x64 、 Win2016系统的Bin</br>
Win2016LPE 为提权主程序</br>
ALPC-TaskSched-LPE 漏洞利用DLL</br>
ExpDLL  exploit dll 编译生成之后放置于ALPC-TaskSched-LPE\resource目录</br>
# 更新历史
2018-09-28</br>
1）由COM启动打印服务改为API启动，避免工具运行时的额外弹窗</br>
2）去掉因进程间通讯造成的防火墙网络连接提示</br>

# 感谢
感谢原作者SandboxEscaper的POC及漏洞writeup</br>
https://github.com/SandboxEscaper/</br>
