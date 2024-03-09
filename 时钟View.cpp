
// 时钟View.cpp: C时钟View 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "时钟.h"
#endif

#include "时钟Doc.h"
#include "时钟View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ImageRotate(CImage* dst, CImage* img, double alpha) {
	//定义区
	int Dx, Dy,ww,bpd;
	double centerX, centerY, rr, theta;					//中心点，rr对角线,thetaXY坐标长度比
	double X1, Y1, X2, Y2,xx,yy;
	BYTE** list, * sc, * lp;
	///////////////////////////////
	Dx = img->GetWidth();
	Dy = img->GetHeight();

	sc = new BYTE[2 * (Dx * img->GetBPP() + 31) / 32 * 4];
	list = new BYTE * [Dy * sizeof(BYTE*)];
	for (int i = 0; i < Dy; i++)
	{
		list[i] = (BYTE*)img->GetPixelAddress(0, i);
	}
	centerX = Dx / 2.0 + 0.5;					//计算中心点坐标
	centerY = Dy / 2.0 + 0.5;
	rr = sqrt(centerX * centerX + centerY * centerY);				//计算对角线长度
	theta = atan(centerY / centerX);
	X1 = fabs(rr * cos(alpha + theta)) + 0.5;
	Y1 = fabs(rr * sin(alpha + theta)) + 0.5;
	X2 = fabs(rr * cos(alpha - theta)) + 0.5;
	Y2 = fabs(rr * sin(alpha - theta)) + 0.5;

	if (X2 > X1) X1 = X2;
	if (Y2 > Y1)Y1 = Y2;
	ww = (int)(2 * X1);//外接矩形宽度，取整
	dst->Destroy();
	dst->Create(ww, (int)(2 * Y1), img->GetBPP());
	bpd = img->GetBPP() / 8;
	double sintheta = sin(alpha);
	double costheta = cos(alpha);
	for (int i = (int)(centerX - X1), Yd = 0; i < centerX + X1; i++, Yd++) {
		if (img->GetBPP() == 8) {
			memset(sc, 255, ww);
		}
		else {
			memset(sc, 255, ww * bpd);
		}
		for (int j = (int)(centerY - Y1), Xd = 0; j < centerY + Y1; j++, Xd++) {
			xx = centerX + costheta * (i - centerX) + sintheta * (i - centerY);
			yy = centerY + sintheta * (i - centerX) + costheta * (i - centerY);
			int x = (int)(xx + 0.5);
			int y = (int)(yy + 0.5);//此处向上取整
			if (x<0 || x>img->GetWidth() || y<0 || y>img->GetHeight()) {
				continue;
			}
			if (x == img->GetWidth()) x--; 
			if (y == img->GetHeight())	y--;
			memcpy(&sc[Xd], &list[y][x * bpd], bpd);
		}
		lp = (BYTE*)dst->GetPixelAddress(0, Yd);
		memcpy(lp, sc, ww * bpd);
	}
	delete[] list;
	delete[] sc;
}
// C时钟View

IMPLEMENT_DYNCREATE(C时钟View, CView)

BEGIN_MESSAGE_MAP(C时钟View, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &C时钟View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// C时钟View 构造/析构

C时钟View::C时钟View() noexcept
{
	// TODO: 在此处添加构造代码

}

C时钟View::~C时钟View()
{
}

BOOL C时钟View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// C时钟View 绘图

void C时钟View::OnDraw(CDC* pDC)
{
	C时钟Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	SYSTEMTIME stm;
	GetLocalTime(&stm);
	int X = 300, Y = 300, R = 270;
	pDC->Ellipse(X - R, Y - R, X + R, Y + R);
	CImage* imglist = new CImage[3];
	imglist[0].Load(L"./img/秒针.png");
	imglist[1].Load(L"./img/分针.png");
	imglist[2].Load(L"./img/时针.png");
	for (int i = 0; i<3; i++) {
		for (int j = 0; j < imglist[i].GetWidth(); j++) {
			for (int k = 0; k < imglist[i].GetHeight(); k++) {
				unsigned char* pucColor = reinterpret_cast <unsigned char*> (imglist[i].GetPixelAddress(j,k));
				pucColor[0] = (pucColor[0] * pucColor[3] + 127) / 255;
				pucColor[1] = (pucColor[1] * pucColor[3] + 127) / 255;
				pucColor[2] = (pucColor[2] * pucColor[3] + 127) / 255;
			}
		}
	}
	imglist[0].Draw(pDC->GetSafeHdc(), X - imglist[0].GetWidth() * 0.5, Y - imglist[0].GetHeight(), imglist[0].GetWidth(), imglist[0].GetHeight());
	imglist[1].Draw(pDC->GetSafeHdc(), X - imglist[1].GetWidth() * 0.5, Y - imglist[1].GetHeight(), imglist[1].GetWidth(), imglist[1].GetHeight());
	imglist[2].Draw(pDC->GetSafeHdc(), X - imglist[2].GetWidth() * 0.5, Y - imglist[2].GetHeight(), imglist[2].GetWidth(), imglist[2].GetHeight());
	ImageRotate(&imglist[0], &imglist[0], acos(-1));
}


// C时钟View 打印


void C时钟View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL C时钟View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void C时钟View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void C时钟View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void C时钟View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void C时钟View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// C时钟View 诊断

#ifdef _DEBUG
void C时钟View::AssertValid() const
{
	CView::AssertValid();
}

void C时钟View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

C时钟Doc* C时钟View::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C时钟Doc)));
	return (C时钟Doc*)m_pDocument;
}
#endif //_DEBUG


// C时钟View 消息处理程序


void C时钟View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	SetTimer(12, 1000, NULL);
}


void C时钟View::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CView::OnTimer(nIDEvent);
	if (nIDEvent == 12) {
		Invalidate(0);
	}
}
