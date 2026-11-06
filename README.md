# kys-ios（非官方 iOS 移植试验）

本仓库是 [scarsty/kys-cpp](https://github.com/scarsty/kys-cpp)（BSD-3-Clause）的非官方 iOS 构建试验分支，
仅用于个人研究与学习，不包含任何游戏素材。

## 构建

推送后由 GitHub Actions（macos-14）自动构建，产出未签名 `kys-unsigned.ipa`（artifact）。

## 本地塞入游戏数据

```powershell
.\inject-data.ps1 -Ipa "kys-unsigned.ipa" -GameDir "你的 game 目录"
```

产出 `kys-with-data.ipa`，可用巨魔（TrollStore）安装。

## 版权

- 引擎代码：BSD-3-Clause，另附条款「金庸题材不得牟利」，见 LICENSE
- 游戏素材：《金庸群侠传》相关素材版权归原权利人，请自行合法获取，勿随仓库分发
