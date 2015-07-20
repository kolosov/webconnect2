import wx
import webconnect

app = wx.App()
frame = wx.Frame(None, -1, 'Pytho Gecko Test')
wc = webconnect.wxWebControl(frame)
wc.OpenURI('www.opennet.ru')
frame.Show()
app.MainLoop()
