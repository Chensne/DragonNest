测试说明：
Demo1 - 不带动态加解密的GPK演示
Demo2 - 带动态加解密的一个简单演示
Demo3 - 带动态加解密的较完整的Client/Server的GPK演示

各示例的正常运行需要一个Web服务器(假设在本地)，用于GPK连接到上面进行升级检查。
测试时进行如下配置:
1. 假设本地服务器的根为http://127.0.0.1
2. 在根下建一个 [/Updates/TestGame/] 子目录
3. 把[Redist\Client\gpk]目录下的文件 复制到 [/Updates/TestGame/] 目录下

像这样配置好后我们的升级URL地址就为http://127.0.0.1/Updates/TestGame

注意：
1. 在linux之类的系统上url是大小写敏感的，请注意程序中url的大小写
2. 请不要在web站点上启用gzip，否则程序不能解析升级信息

演示程序编译好后，运行之前需要注意以下事项：
1. 请确认 [Redist\Server\DynCodeBin] 中的动态代码是否已经放到了程序中指定的路径下，Server端会用到
2. Client端主程序目录下需要建一个名叫gpk的子目录，然后放上 [Redist\Client\gpk] 中的AutoUpdate.dll和SddynDll.dll。（其他文件可以不放，程序启动的时候会自动升级下来）
3. 请确保Client端和Server端的主程序能找到他们需要的GPKitCltDll.dll和GPKitSvr.dll。
