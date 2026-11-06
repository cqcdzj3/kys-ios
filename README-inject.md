# 把游戏数据塞进未签名 IPA（Windows 本地运行）

用法：

```powershell
.\inject-data.ps1 -Ipa "C:\path\kys-unsigned.ipa" -GameDir "C:\kysbuild\kyscpp\game"
```

产出：`kys-with-data.ipa`（同目录），用巨魔直接安装即可。

脚本做的事：
1. 解压 ipa
2. 把 `-GameDir` 下的全部内容复制到 `Payload/kys.app/game/`
3. 重新打包成 ipa（不压缩，速度快）

> 注意：游戏数据是第三方素材，仅在本地处理，不要上传到仓库或公开发布。
