# NoDesktopWM
Remove 'Activate Windows' watermark at desktop.\
Tested on Windows 10 20H2 and Windows 11 22000.51

https://user-images.githubusercontent.com/12496720/124856492-924da900-dfe5-11eb-9dc3-9215f2e82af7.mp4




## How does it work
After windows 8, Explorer create 'Activate Windows' window at special Z-order using [Band](https://blog.adeltax.com/window-z-order-in-windows-10/).\
In NoDesktopWM.dll, Find 'Activate Windows' window using 'EnumChildWindows' and hide it.\
'NoDesktopWMUI.exe' inject 'NoDesktopWM.dll' to explorer.exe

## Known bug
After a while, an afterimage of the watermark appears on the desktop. (only first time)\
This afterimage is not top-most and drag mouse at afterimage or move window to afterimage, afterimage will removed.

https://user-images.githubusercontent.com/12496720/124856882-4cddab80-dfe6-11eb-995f-234f1791f98f.mp4
