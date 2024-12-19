因为gpk具有很强的抗调试能力，所以整合gpk后可能无法调试，如果要调试，请使用debug目录中的文件替换(包括client和Server)。
debug目录中的gpk去除了全部的反外挂功能，仅保留了动态代码功能方便开发者调试。发布请使用Release中的相关文件。
Release和Debug目录的区别：
\Server\DynCodeBin中的所有动态代码文件
\Client\gpk\gpk.s
其余文件相同。
