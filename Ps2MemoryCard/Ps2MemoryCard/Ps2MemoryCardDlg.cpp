/*  OpenWQSG - Ps2MemoryCard
*  Copyright (C) WQSG <wqsg@qq.com>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/

// Ps2MemoryCardDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Ps2MemoryCard.h"
#include "Ps2MemoryCardDlg.h"
#include "VPscMc.h"
#include "SelDir.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPs2MemoryCardDlg 对话框

int CALLBACK ItemSort( LPARAM a , LPARAM b , LPARAM c )
{
	SItemSortData* pData1 = (SItemSortData*)((c&0xF0000000)?a:b);
	SItemSortData* pData2 = (SItemSortData*)((c&0xF0000000)?b:a);

	switch ( c & 0x0FFFFFFF )
	{
	case 1:
		return pData1->m_nSize - pData2->m_nSize;
		break;
	case 2:
		return pData1->m_strFileName.Compare( pData2->m_strFileName );
		break;
	}

	return pData1->m_strTitleName.Compare( pData2->m_strTitleName );
}

CPs2MemoryCardDlg::CPs2MemoryCardDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPs2MemoryCardDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPs2MemoryCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cList1);
	DDX_Control(pDX, IDC_LIST2, m_cList2);
}

BEGIN_MESSAGE_MAP(CPs2MemoryCardDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_MC1, &CPs2MemoryCardDlg::OnBnClickedButtonOpenMc1)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_MC1, &CPs2MemoryCardDlg::OnBnClickedButtonCreateMc1)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_MC1, &CPs2MemoryCardDlg::OnBnClickedButtonSaveMc1)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_BIN1, &CPs2MemoryCardDlg::OnBnClickedButtonSaveBin1)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_PSU1, &CPs2MemoryCardDlg::OnBnClickedButtonImportPsu1)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_PSU1, &CPs2MemoryCardDlg::OnBnClickedButtonExportPsu1)
	ON_BN_CLICKED(IDC_BUTTON_SEL_PSU_DIR, &CPs2MemoryCardDlg::OnBnClickedButtonSelPsuDir)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CPs2MemoryCardDlg::OnBnClickedButtonAbout)
	ON_BN_CLICKED(IDC_BUTTON1, &CPs2MemoryCardDlg::OnBnClickedButton1)

	ON_NOTIFY(HDN_ITEMCLICK, 0, &CPs2MemoryCardDlg::OnHdnItemclickList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST1, &CPs2MemoryCardDlg::OnLvnDeleteitemList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST2, &CPs2MemoryCardDlg::OnLvnDeleteitemList)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &CPs2MemoryCardDlg::OnBnClickedButtonClearLog)
END_MESSAGE_MAP()


// CPs2MemoryCardDlg 消息处理程序

BOOL CPs2MemoryCardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	HDITEM hi = {};
	hi.mask = HDI_FORMAT;

	m_cList1.SetExtendedStyle( m_cList1.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_cList1.InsertColumn( 0 , L"存档名" , 0 , 450 );
	m_cList1.InsertColumn( 1 , L"size" , 0 , 40 );
	m_cList1.InsertColumn( 2 , L"原名" , 0 , 180 );

	m_cList1.GetHeaderCtrl()->GetItem( 0 , &hi );
	hi.fmt |= HDF_SORTUP;
	m_cList1.GetHeaderCtrl()->SetItem( 0 , &hi );

	m_cList2.SetExtendedStyle( m_cList2.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_cList2.InsertColumn( 0 , L"存档名" , 0 , 450 );
	m_cList2.InsertColumn( 1 , L"size" , 0 , 40 );
	m_cList2.InsertColumn( 2 , L"原名" , 0 , 180 );

	m_cList2.GetHeaderCtrl()->GetItem( 0 , &hi );
	hi.fmt |= HDF_SORTUP;
	m_cList2.GetHeaderCtrl()->SetItem( 0 , &hi );

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPs2MemoryCardDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CPs2MemoryCardDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPs2MemoryCardDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}

void CPs2MemoryCardDlg::OnBnClickedButtonOpenMc1()
{
	// TODO: 在此添加控件通知处理程序代码
	static CWQSGFileDialog_Open dlg( L"*.ps2;*.bin|*.ps2;*.bin||" );
	if( IDOK != dlg.DoModal() )
		return;

	if( !m_Mc1.LoadMc( dlg.GetPathName() ) )
	{
		SetDlgItemText( IDC_EDIT1 , L"" );
		m_Mc1.FormatMc();
		UpdateMcUI();
		MessageBox( L"加载记忆卡失败" );
		return;
	}

	SetDlgItemText( IDC_EDIT1 , dlg.GetPathName() );
	UpdateMcUI();
}

void CPs2MemoryCardDlg::OnBnClickedButtonCreateMc1()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText( IDC_EDIT1 , L"" );
	if( !m_Mc1.FormatMc() )
	{
		MessageBox( L"暂时不支持" );
	}
	UpdateMcUI();
}

void CPs2MemoryCardDlg::OnBnClickedButtonSaveMc1()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !m_Mc1.isOpen() )
		return ;

	static CWQSGFileDialog_Save dlg( L"*.ps2|*.ps2||" , L"ps2" );
	if( IDOK != dlg.DoModal() )
		return;

	if( m_Mc1.SaveMc( dlg.GetPathName() ) )
	{
		SetDlgItemText( IDC_EDIT1 , dlg.GetPathName() );
	}
	else
	{
		MessageBox( L"保存记忆卡失败" );
	}
}

void CPs2MemoryCardDlg::OnBnClickedButtonImportPsu1()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !m_Mc1.isOpen() )
		return ;

	CString strDstPath;
	GetDlgItemText( IDC_EDIT2 , strDstPath );
	if( strDstPath.GetLength() == 0 )
		return ;

	strDstPath += L"\\";

	bool bShow = true;
	int iOk = 0;
	int iError = 0;

	POSITION pos = m_cList2.GetFirstSelectedItemPosition();
	while(true)
	{
		const int iIndex = m_cList2.GetNextSelectedItem( pos );
		if( iIndex == -1 )
			break ;

		if( bShow )
		{
			if( MessageBox( L"如有重复的存档,将会被覆盖,确定要导入吗?" , NULL , MB_YESNO ) != IDYES )
				return;

			bShow = false;
		}

		const CString str = m_cList2.GetItemText( iIndex , 2 );

		m_Mc1.Bak();
		const bool bOk = m_Mc1.Import_Psu( strDstPath + str + L".psu" );

		if( bOk )
		{
			iOk++;
		}
		else
		{
			iError++;
			m_Mc1.UnBak();

			m_strLog.AppendFormat( L"导入 %s 失败\r\n" , str.GetString() );
			SetDlgItemText( IDC_EDIT_LOG , m_strLog );
		}
	}

	if( iOk || iError )
	{
		UpdateMcUI();

		MessageBox( L"导入完毕!" );
	}
}

void CPs2MemoryCardDlg::OnBnClickedButtonExportPsu1()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !m_Mc1.isOpen() )
		return ;

	CString strDstPath;
	GetDlgItemText( IDC_EDIT2 , strDstPath );
	if( strDstPath.GetLength() == 0 )
		return ;

	strDstPath += L"\\";

	bool bShow = true;
	int iOk = 0;
	int iError = 0;

	POSITION pos = m_cList1.GetFirstSelectedItemPosition();
	while(true)
	{
		const int iIndex = m_cList1.GetNextSelectedItem( pos );
		if( iIndex == -1 )
			break ;

		if( bShow )
		{
			if( MessageBox( L"如有重复的存档,将会被覆盖,确定要导出吗?" , NULL , MB_YESNO ) != IDYES )
				return;

			bShow = false;
		}

		const CString str = m_cList1.GetItemText( iIndex , 2 );

		char* pName = WQSG_W_char( str.GetString() , 932 );
		const bool bOk = m_Mc1.Export_Psu( strDstPath + str + L".psu" , pName );
		delete[]pName;

		if( bOk )
		{
			iOk++;
		}
		else
		{
			iError++;
			m_strLog.AppendFormat( L"导出 %s 失败\r\n" , str.GetString() );
			SetDlgItemText( IDC_EDIT_LOG , m_strLog );
		}
	}

	if( iOk || iError )
	{
		UpdateDirUI();

		MessageBox( L"导出完毕!" );
	}
}

void CPs2MemoryCardDlg::UISort( CListCtrl& a_cList )
{
	CHeaderCtrl& cc = *a_cList.GetHeaderCtrl();

	HDITEM hi = {};
	hi.mask = HDI_FORMAT;

	u32 uFlag = 0;
	for( int i = 0 ; i < cc.GetItemCount() ; ++i )
	{
		cc.GetItem( i , &hi );

		if( hi.fmt & (HDF_SORTUP|HDF_SORTDOWN) )
		{
			uFlag = i;
			break;
		}
	}

	if( hi.fmt & HDF_SORTDOWN )
		uFlag |= 0xF0000000;

	a_cList.SortItems( &ItemSort , uFlag );
}

void CPs2MemoryCardDlg::UpdateMcUI()
{
	m_cList1.DeleteAllItems();
	if( !m_Mc1.isOpen() )
		return ;

	std::vector<CVPscMc::SFileInfo> files;
	if( !m_Mc1.GetFiles( files , "" ) )
	{
		MessageBox( L"获取文件列表失败" );
		return;
	}

	m_cList1.SetRedraw( FALSE );
	for( std::vector<CVPscMc::SFileInfo>::iterator it = files.begin() ;
		it != files.end() ; ++it )
	{
		const CVPscMc::SFileInfo& info = *it;

		SItemSortData data;

		CWQSG_memFile mf;
		if( m_Mc1.Vmc_ReadFile( mf , info.szName , "icon.sys" ) )
		{
			mf.Seek( 0xC0 );

			char buf[0x100];
			if( sizeof(buf) != mf.Read( buf , sizeof(buf) ) )
			{
				MessageBox( L"取存档文件名失败" );
				break;
			}

			WCHAR* pTitle = WQSG_char_W( buf , 932 );
			data.m_strTitleName = pTitle;
			delete[]pTitle;
		}
		else
		{
			WCHAR* pX = WQSG_char_W( info.szName , 932 );
			data.m_strTitleName = pX;
			delete[]pX;
		}

		data.m_nSize = info.uSize;

		WCHAR* pX = WQSG_char_W( info.szName , 932 );
		data.m_strFileName = pX;
		delete[]pX;

		UIInsert( m_cList1 , data );
	}
	//--------------------------
	UISort( m_cList1 );
	//--------------------------
	m_cList1.SetRedraw( TRUE );

	u32 count = 0;
	if( m_Mc1.GetFreeClusterCount( count ) )
	{
		CStringW str;
		str.Format( L"空闲块 = %d KB , size = %d" , count , m_Mc1.GetPreClusterSize() * count );
		OutputDebugString( str );
		OutputDebugString( L"\r\n" );
		SetDlgItemText( IDC_EDIT_MC_INFO , str );
	}
	else
	{
		CStringW str;
		str.Format( L"取空闲块失败 , 空闲块 = %d KB , size = %d" , count , m_Mc1.GetPreClusterSize() * count );
		OutputDebugString( str );
		OutputDebugString( L"\r\n" );
		SetDlgItemText( IDC_EDIT_MC_INFO , str );
	}
}

void CPs2MemoryCardDlg::UpdateDirUI()
{
	m_cList2.DeleteAllItems();
	CString strDir;
	GetDlgItemText( IDC_EDIT2 , strDir );

	if( strDir.GetLength() == 0 )
		return;

	strDir += L"\\";

	WIN32_FIND_DATAW findData;
	const HANDLE hFind = FindFirstFileW( strDir + L"*.psu" , &findData );
	if( hFind == INVALID_HANDLE_VALUE )
		return;

	m_cList2.SetRedraw( FALSE );
	do
	{
		if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			continue;

		SPsuData files;
		if( !Load_Psu( strDir + findData.cFileName , files ) )
			continue;

		SItemSortData data;

		TPsuFile::iterator it = files.m_files.find( "icon.sys" );
		if( it != files.m_files.end() )
		{
			CWQSG_memFile mf;
			mf.Write( &it->second.at(0) , it->second.size() );

			mf.Seek( 0xC0 );

			char buf[0x100];
			if( sizeof(buf) != mf.Read( buf , sizeof(buf) ) )
			{
				MessageBox( L"取存档文件名失败" );
				break;
			}

			WCHAR* pTitle = WQSG_char_W( buf , 932 );
			data.m_strTitleName = pTitle;
			delete[]pTitle;
		}
		else
		{
			WCHAR* pX = WQSG_char_W( files.m_strName , 932 );
			data.m_strTitleName = pX;
			delete[]pX;
		}

		data.m_nSize = files.m_files.size();

		WCHAR* pX = WQSG_char_W( files.m_strName , 932 );
		data.m_strFileName = pX;
		delete[]pX;

		UIInsert( m_cList2 , data );
	}
	while( FindNextFileW( hFind , &findData ) );
	//--------------------------
	UISort( m_cList2 );
	//--------------------------
	m_cList2.SetRedraw( TRUE );
}

void CPs2MemoryCardDlg::UIInsert( CListCtrl& a_cList , const SItemSortData& a_Data )
{
	const int iIndex = a_cList.InsertItem( a_cList.GetItemCount() , a_Data.m_strTitleName );

	if( iIndex == -1 )
	{
		MessageBox( L"添加失败" );
		return;
	}

	SItemSortData* pData = new SItemSortData;
	a_cList.SetItemData( iIndex , (DWORD_PTR)pData );

	*pData = a_Data;

	CString str;
	str.Format( L"%d" , pData->m_nSize );
	a_cList.SetItemText( iIndex , 1 , str );
	a_cList.SetItemText( iIndex , 2 , pData->m_strFileName );
}
// 00833A00 map? 00000000 ,总之全FF
// 0083DF00 MAP  0000A500
void CPs2MemoryCardDlg::OnBnClickedButtonAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strAppName;
	strAppName.LoadString( IDS_APP_NAME );

	CString strAppVer;
	strAppVer.LoadString( IDS_APP_VER );

	CString strAuthor1;
	strAuthor1.LoadString( IDS_APP_AUTHOR );

	CString strAuthor2;
	strAuthor2.LoadString( IDS_APP_AUTHOR2 );

	WQSG_About( m_hIcon , m_hWnd , L"关于本软件" , strAppName + L"\r\nv" + strAppVer ,
		L"项目svn : <A HREF=\"http://code.google.com/p/openwqsg\">http://code.google.com/p/openwqsg</A>\r\n"
		L"依赖库svn : <A HREF=\"http://code.google.com/p/wqsglib\">http://code.google.com/p/wqsglib</A>\r\n                 <A HREF=\"http://wqsg.ys168.com\">http://wqsg.ys168.com</A>\r\n" ,
		strAuthor2 + L"(" + strAuthor1 + L")" );
}

bool CPs2MemoryCardDlg::Load_Psu( const CStringW& a_strFile , SPsuData& a_Files )
{
	CWQSG_File fp;
	if( !fp.OpenFile( a_strFile.GetString() , 1 , 3 ) )
		return false;

	SPsu_header psu_head = {};
	if( sizeof(psu_head) != fp.Read( &psu_head , sizeof(psu_head) ) )
		return false;

	if( !(psu_head.attr & DF_DIRECTORY) )
		return false;

	if( !(psu_head.attr & DF_EXISTS) )
		return false;

	{
		static const SPsu_header psu_x = {};

		if( psu_head.unknown_1_u16 != 0 || psu_head.unknown_2_u64 != 0 || psu_head.EMS_used_u64 != 0 )
			return false;

		if( memcmp( psu_head.unknown_3_24_bytes , psu_x.unknown_3_24_bytes , sizeof(psu_x.unknown_3_24_bytes) ) != 0 ||
			memcmp( psu_head.unknown_4_416_bytes , psu_x.unknown_4_416_bytes , sizeof(psu_x.unknown_4_416_bytes) ) != 0)
			return false;
	}


	a_Files.m_strName = (const char*)psu_head.name;
	a_Files.m_files.clear();

	for( u32 i = 0 ; i < psu_head.size ; ++i )
	{
		SPsu_header head1 = {};
		if( sizeof(head1) != fp.Read( &head1 , sizeof(head1) ) )
			return false;

		{
			static const SPsu_header psu_x = {};

			if( head1.unknown_1_u16 != 0 || head1.unknown_2_u64 != 0 || head1.EMS_used_u64 != 0 )
				return false;

			if( memcmp( head1.unknown_3_24_bytes , psu_x.unknown_3_24_bytes , sizeof(psu_x.unknown_3_24_bytes) ) != 0 ||
				memcmp( head1.unknown_4_416_bytes , psu_x.unknown_4_416_bytes , sizeof(psu_x.unknown_4_416_bytes) ) != 0)
				return false;
		}

		if( head1.attr & DF_DIRECTORY )
		{
			if( head1.size == 0 )
				continue;

			return false;
		}

		if( !(head1.attr & DF_EXISTS) )
			return false;

		std::vector<u8>& buff = a_Files.m_files[std::string((const char*)head1.name)];

		if( head1.size )
		{
			buff.resize( head1.size , 0 );

			if( head1.size != fp.Read( &buff[0] , head1.size ) )
				return false;

			const u32 p = head1.size % 0x400;

			if( p )
				fp.Seek( fp.Tell() + (0x400 - p) );

			if( 0 )
			{
				CWQSG_File FP;
				if( FP.OpenFile( L"D:\\WQSG\\ICON.BIN" , 4 , 3 ) )
				{
					FP.Write( &buff[0] , head1.size );
					FP.Close();
				}
			}
		}
	}

	return true;
}

void CPs2MemoryCardDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	static CWQSGFileDialog_Open dlg1( L"*.psu|*.psu||" );
	if( IDOK != dlg1.DoModal() )
		return;

	static CWQSGFileDialog_Open dlg2( L"*.psu|*.psu||" );
	if( IDOK != dlg2.DoModal() )
		return;

	SPsuData psu1;
	SPsuData psu2;

	if( !Load_Psu( dlg1.GetPathName() , psu1 ) )
	{
		MessageBox( L"读取失败" , dlg1.GetPathName() );
		return;
	}

	if( !Load_Psu( dlg2.GetPathName() , psu2 ) )
	{
		MessageBox( L"读取失败" , dlg2.GetPathName() );
		return;
	}

	if( psu1.m_strName != psu2.m_strName )
	{
		MessageBox( L"存档不同" );
		return;
	}

	if( psu1.m_files.size() != psu2.m_files.size() )
	{
		MessageBox( L"文件数不等" );
		return;
	}

	for( TPsuFile::const_iterator it1 = psu1.m_files.begin() ;
		it1 != psu1.m_files.end() ; ++it1 )
	{
		TPsuFile::const_iterator it2 = psu2.m_files.find(it1->first);
		if( it2 == psu2.m_files.end() )
		{
			MessageBox( L"文件名不等" );
			return;
		}

		if( it1->second.size() != it2->second.size() )
		{
			CString str;
			str = it1->first.c_str();
			MessageBox( L"数据长度不等" , str );
			return;
		}

		if( memcmp( &(it1->second[0]) , &(it2->second[0]) , it2->second.size() ) )
		{
			CString str;
			str = it1->first.c_str();
			MessageBox( L"数据不等" , str );
			return;
		}
	}

	MessageBox( L"相等" );
}

void CPs2MemoryCardDlg::OnBnClickedButtonSaveBin1()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !m_Mc1.isOpen() )
		return ;

	static CWQSGFileDialog_Save dlg( L"*.bin|*.bin||" , L"bin" );
	if( IDOK != dlg.DoModal() )
		return;

	if( m_Mc1.SaveMcNoEcc( dlg.GetPathName() ) )
	{
		SetDlgItemText( IDC_EDIT1 , dlg.GetPathName() );
	}
	else
	{
		MessageBox( L"保存记忆卡失败" );
	}
}

void CPs2MemoryCardDlg::OnHdnItemclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if( phdr->iButton != 0 )
		return;

	CHeaderCtrl& cc = *(CHeaderCtrl*)CHeaderCtrl::FromHandle( phdr->hdr.hwndFrom );

	HDITEM hi = {};
	hi.mask = HDI_FORMAT;

	cc.GetItem( phdr->iItem , &hi );
	if( hi.fmt & (HDF_SORTUP|HDF_SORTDOWN) )
	{
	}
	else
	{
		for( int i = 0 ; i < cc.GetItemCount() ; ++i )
		{
			cc.GetItem( i , &hi );

			const int oldFmt = hi.fmt & (HDF_SORTUP|HDF_SORTDOWN);
			if( oldFmt )
			{
				hi.fmt &= ~(HDF_SORTUP|HDF_SORTDOWN);
				cc.SetItem( i , &hi );
				break;
			}
		}
		cc.GetItem( phdr->iItem , &hi );
	}

	const int oldFmt = hi.fmt & (HDF_SORTUP|HDF_SORTDOWN);
	hi.fmt &= ~(HDF_SORTUP|HDF_SORTDOWN);
	if( oldFmt & HDF_SORTUP )
		hi.fmt |= HDF_SORTDOWN;
	else
		hi.fmt |= HDF_SORTUP;

	cc.SetItem( phdr->iItem , &hi );

	if( m_cList1.GetHeaderCtrl() == &cc )
		UISort( m_cList1 );
	else if( m_cList2.GetHeaderCtrl() == &cc )
		UISort( m_cList2 );
}

void CPs2MemoryCardDlg::OnLvnDeleteitemList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CListCtrl* pList = (CListCtrl*)CListCtrl::FromHandle( pNMLV->hdr.hwndFrom );

	delete (SItemSortData*)pList->GetItemData( pNMLV->iItem );
}


void CPs2MemoryCardDlg::OnBnClickedButtonSelPsuDir()
{
	// TODO: 在此添加控件通知处理程序代码
	CString oldDir;

	GetDlgItemText( IDC_EDIT2 , oldDir );

	CSelDir dlg( oldDir );
	if( dlg.DoModal() != IDOK )
		return;

	SetDlgItemText( IDC_EDIT2 , dlg.GetSel() );

	UpdateDirUI();
}

void CPs2MemoryCardDlg::OnBnClickedButtonClearLog()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strLog = L"";
	SetDlgItemText( IDC_EDIT_LOG , m_strLog );
}
