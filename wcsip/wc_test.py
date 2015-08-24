import wx
import wx.lib.agw.aui
import webconnect


class MyFrame(wx.Frame):

#    def __init__(self, parent, id=-1, title='wx.aui Test', pos=wx.DefaultPosition, size=(800, 600), style=wx.DEFAULT_FRAME_STYLE):
#        wx.Frame.__init__(self, parent, id, title, pos, size, style)


	def __init__(self, parent, id=-1, title='Gecko test', pos=wx.DefaultPosition, size=(800,600), style=wx.DEFAULT_FRAME_STYLE):
		wx.Frame.__init__(self, parent, id, title, pos, size, style)

#		self._mgr = wx.aui.AuiManager(self)
		self._mgr = wx.lib.agw.aui.AuiManager(self)
# create several text controls
		toolbar = wx.lib.agw.aui.AuiToolBar(self, -1, wx.DefaultPosition, wx.DefaultSize,
                                             wx.lib.agw.aui.AUI_TB_DEFAULT_STYLE)

		#m_urlbar = new wxComboBox(toolbar, wxID_URL, wxT(""), wxPoint(0,0), wxSize(850,18));
		#urlbar = wx.ComboBox(toolbar, -1, pos=(0, 0), size=(850, 18), choices=authors, style=wx.CB_READONLY)
		urlbar = wx.ComboBox(toolbar, -1, pos=(0, 0), size=(850, 18))

		#toolbar->AddControl(m_urlbar, wxT("Location"));
		toolbar.AddControl(urlbar,'Location')
		toolbar.Realize()
		
		#m_browser = new wxWebControl(this, wxID_WEB, wxPoint(0,0), wxSize(800,600));
		self._browser = webconnect.wxWebControl(self)

		#add pane
		self._mgr.AddPane(urlbar, wx.TOP, 'Toolbar')
		self._mgr.AddPane(self._browser, wx.CENTER, 'Browser')
		# tell the manager to 'commit' all the changes just made
		self._mgr.Update()
		self._browser.OpenURI('www.opennet.ru')

#		self.Bind(wx.EVT_CLOSE, self.OnClose)


app = wx.App()
frame = MyFrame(None)
frame.Show()
app.MainLoop()

#app = wx.App()
#frame = wx.Frame(None, -1, 'Pytho Gecko Test')
#wc = webconnect.wxWebControl(frame)
#wc.OpenURI('www.opennet.ru')
#frame.Show()
#app.MainLoop()
