# Perfect-Visible-Check-External-

## 1) Run EXE after start CSGO

## 2) Use at own risk on VAC secured servers. Or add junks to code!

## 3) Offsets in 'Main.h'

## 4) In hook function need use simple Vector class without any methods or you get game crashes!
```sh
struct SimpleVector
{
	float x, y, z;
};
```

## 5) ForceFullUpdate can cause crash game! Added this check:
```sh
bool IsInForceFullUpdate    = *(int*)(Vars->m_dwClientState + Vars->m_nDeltaTick) == -1;
```
Crashes are not gone, but are much less


## Video-Demo
[![Video-Demo](https://img.youtube.com/vi/fqcnjO-g6Qo/0.jpg)](https://www.youtube.com/watch?v=fqcnjO-g6Qo)
