# 作者主站
http://rlzl.net

# 这是什么
这是一个作者自用的在yande.re上保存图片的服务程序.<br>
它会开启一个网页服务,通过访问网页,然后输入yande上的编号自动抓取png或jpg然后下载.<br>
只需要在yande上找到想要的图,输入编号即可.<br>

# 为什么不做成整站爬图?
以前做过,下载超过5000-6000的图,但这也只是yande上小小的一部分.<br>
最大的问题是,下载后需要人工手动删除自己不喜欢的,或图片不是cg的.<br>
这部分工作比自动抓取累多了.还不如用搜索功能排除.<br>
最坑的是每次3G,4G的大小会被删的只剩下几百MB.<br>

# 它的优势是什么.
- 服务端支持代理软件(翻墙,反正不翻墙我访问不了)
- 我通过frp曝出端口给云服务器,即便出门在外也能远程访问抓图.
- 筛选图不用手动一张张去点击大图然后保存了.只要输个编号即可.
- 有时候弄多了自己都忘了弄过哪些,不用如果输入重复的会有提示,并不会重复下载保存.
- 当图片有png存在时优先提取png忽略jpg.(yande的趋势是png为高清图)
- 所有抓取的信息都留在数据库中,可二次开发功能.(前提得会)

# 它是什么程序
它是一个我用VS2019开发的C++程序.<br>
开发环境为Win10 + MSSQL2019<br>
使用了httplib,libcurl,jsoncpp第三方库.<br>
程序会开启一个简单的web服务,一些post监听.内部两个线程用于抓取链接和下载图片.<br>

# 平台支持性?
支持`Windows7`或`Windows Server 2016`及以上<br>
数据要求为`SQL Server2008`及以上,建议使用`Express`版本.<br>
(免费,可用于生产环境,功能小对服务器硬件压力低)<br>
> 注意默认都是64位的系统,如果需要32位程序需要自己编译一下,将lib的库替换成x86版本.<br>
> 还有当前默认为`/MD`模式,如果不想要vsruntime也需要将库都改为`/MT`模式

### 网页页面怎么那么烂
因为我不会前端,都是到处抄的,前端也只是接触了WebGL才有些了解.<br>
HTML和CSS都得超别人得然后改,有些基础的还得到处百度.<br>
做过Playcanvas和Cocos Creator.<br>
如果用webgl引擎做界面倒是可以很漂亮,但那种页面是时时更新的,有帧率的存在,对cpu和gpu有消耗,考虑到这个业务场景,算了.<br>

# 怎么使用
参见[wiki说明](https://github.com/dreamrz/Yande.re_AutoServer/wiki) <br>
另外我会发布发行版 [这里直接下载](https://github.com/dreamrz/Yande.re_AutoServer/releases) 通常是64位系统的exe包.<br>