/*  OpenWQSG - WQSG_NDS
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

// WQSG_NDSDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WQSG_NDS.h"
#include "WQSG_NDSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWQSG_NDSDlg �Ի���


#define FSI_ICON_UNK	0
#define FSI_ICON_TILE	1
#define FSI_ICON_SOUND	2
#define FSI_ICON_ANIME	3
#define FSI_ICON_NDS	4
#define FSI_ICON_PAL	5
#define FSI_ICON_TEXT	6
#define FSI_ICON_MAP	7
#define FSI_ICON_LZ		8
#define FSI_ICON_HAN	9
#define FSI_ICON_OT		10

#define FSI_SUB_FILENAME	0
#define FSI_SUB_FILEID		1
#define FSI_SUB_FILEOFFSET	2
#define FSI_SUB_FILESIZE	3
#define FSI_SUB_FILEHEADER	4
#define FSI_SUB_FILEINFO	5



CWQSG_NDSDlg::CWQSG_NDSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWQSG_NDSDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWQSG_NDSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FSILIST, m_NdsFSI);
}

BEGIN_MESSAGE_MAP(CWQSG_NDSDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_NOTIFY(HDN_GETDISPINFO, 0, &CWQSG_NDSDlg::OnHdnGetdispinfoFsilist)
	ON_COMMAND(ID_RomOpen, &CWQSG_NDSDlg::OnRomopen)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_FSILIST, &CWQSG_NDSDlg::OnLvnItemActivateFsilist)
	ON_NOTIFY(LVN_DELETEITEM, IDC_FSILIST, &CWQSG_NDSDlg::OnLvnDeleteitemFsilist)
	ON_COMMAND(ID_RomClose, &CWQSG_NDSDlg::OnRomclose)
	ON_BN_CLICKED(IDC_BUTTON_UPDIR, &CWQSG_NDSDlg::OnBnClickedButtonUpdir)
END_MESSAGE_MAP()// CWQSG_NDSDlg ��Ϣ��������
BOOL CWQSG_NDSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ����Ӷ���ĳ�ʼ������
	m_NdsFSI.ModifyStyle( LVS_SORTASCENDING | LVS_SORTDESCENDING |
		LVS_ICON | LVS_SMALLICON | LVS_LIST | LVS_EDITLABELS | LVS_NOSORTHEADER ,
		LVS_REPORT | LVS_SHOWSELALWAYS/*|LVS_OWNERDRAWFIXED*/);

	m_NdsFSI.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP );

	CString header, text;
	header.LoadString(IDS_NDSFSIHEADER);

	const BYTE nFormat[] = {
		LVCFMT_LEFT , LVCFMT_RIGHT , LVCFMT_LEFT ,
		LVCFMT_RIGHT , LVCFMT_LEFT , LVCFMT_LEFT };

	const BYTE nWidth[] = {
		144 , 60 , 70 ,
		92 , 64 , 64 };

	for( int i=0 ; i < (sizeof(nWidth)/sizeof(*nWidth)) ; i++ )
	{
		AfxExtractSubString( text , header , i );
		m_NdsFSI.InsertColumn( i , text , nFormat[i] , nWidth[i] );
	}

	//CView *pView = m_pParentWnd;
	//m_pParentWnd = pView->GetParent();
#if 0
	GetWindowText(header);
	header.Append(m_pTileView->GetDocument()->GetTitle());
	SetWindowText(header);
#endif
#if 0
	m_hImageList = ImageList_Create(16, 16, ILC_COLOR8, 0, 0);
	DWORD dwSize;
	HANDLE hHandle = LoadRes(IDR_NDSFSI, dwSize, _T("CRYSTAL"));
	LPPICTURE pPix= LoadPic(hHandle, dwSize, TRUE);
	HBITMAP hBitMap;
	pPix->get_Handle((OLE_HANDLE*)&hBitMap);
	ImageList_AddMasked(m_hImageList, hBitMap, 0);
	pPix->Release();
#endif
	/*
	m_hImageList = ImageList_LoadBitmap(AfxGetInstanceHandle(),
	MAKEINTRESOURCE(IDB_NDSFSI),
	16, LR_LOADTRANSPARENT, 0xc0c0c0);*/
	//m_NdsFSI.SetImageList(&il, LVSIL_SMALL);
	m_NdsFSI.SendMessage( LVM_SETIMAGELIST , LVSIL_SMALL , (LPARAM)m_hImageList );

	SetIcon(ImageList_GetIcon(m_hImageList, FSI_ICON_NDS, 0), FALSE);

	OnLoadFSI();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի���������С����ť������Ҫ����Ĵ��������Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó����⽫�ɿ���Զ���ɡ�
void CWQSG_NDSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CWQSG_NDSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWQSG_NDSDlg::OnClose()
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ

	CDialog::OnClose();
}

void CWQSG_NDSDlg::OnOK()
{
	// TODO: �ڴ�����ר�ô����/����û���

	CDialog::OnOK();
}

void CWQSG_NDSDlg::OnCancel()
{
	// TODO: �ڴ�����ר�ô����/����û���

	CDialog::OnCancel();
}

void CWQSG_NDSDlg::OnSize(UINT nType, int cx, int cy)
{
	if(m_NdsFSI.GetSafeHwnd())
	{
		CRect rc; m_NdsFSI.GetWindowRect(&rc);
		ScreenToClient(rc);
		m_NdsFSI.SetWindowPos(NULL,
			0, 0,
			cx-rc.left,
			cy-rc.top,
			SWP_NOMOVE);
	}

	CDialog::OnSize(nType, cx, cy);

	// TODO: �ڴ˴�������Ϣ�����������
}

void CWQSG_NDSDlg::OnHdnGetdispinfoFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHDDISPINFO pDispInfo = reinterpret_cast<LPNMHDDISPINFO>(pNMHDR);
	// TODO: �ڴ����ӿؼ�֪ͨ�����������

	UINT FileID = (UINT)pDispInfo->iItem; CString text;
	if(FileID>=HEADERCOUNT)
	{
#if 0
		FileID = GETFILEID(pDispInfo->lParam);
		NDSFILEREC *pFileRec;
		//memcpy((BYTE*)&filerec, m_pRom+m_pHeader->fat_offset+(FileID<<3), sizeof(filerec));
		pFileRec = (NDSFILEREC*)(m_Rom.GetBuf()+m_pHeader->Fat_Offset+(FileID<<3));
//#if 0
		if(pDispInfo->mask & LVIF_TEXT)
		{
			switch(pDispInfo->iSubItem)
			{
			case FSI_SUB_FILEID://ID
				text.Format(_T("%04d"), FileID);
				break;
			case FSI_SUB_FILEOFFSET://Offset
				text.Format(_T("%08X"), pFileRec->top);
				break;
			case FSI_SUB_FILESIZE://Size
				{
					text.Format(_T("%d"), pFileRec->bottom-pFileRec->top);
					int nPos=text.GetLength();
					while(nPos>3)	{nPos-=3; text.Insert(nPos, _T(','));}
				}break;
			case FSI_SUB_FILEHEADER://HeaderInfo
				{
					LVITEM lvi; memset(&lvi, 0, sizeof(lvi));
					lvi.iItem = pDispInfo->iItem; lvi.mask = LVIF_IMAGE;
					m_NdsFSI.GetItem(&lvi);
					if(lvi.iImage==FSI_ICON_TEXT)
					{
						UINT nSize = pFileRec->bottom-pFileRec->top;
						if(nSize>0x10) nSize=0x10;
						//text.Append(m_pRom+pFileRec->top, nSize);
						LPTSTR p = text.GetBuffer(0x20);
						memset(p, 0, sizeof(TCHAR)*0x20);
						MultiByteToWideChar(932, 0, (LPCSTR)(m_Rom.GetBuf()+pFileRec->top), nSize,
							p, 0x20);
						text.ReleaseBuffer(0x20);
					}else
					{
						text = GetSubHeader(pFileRec->top, lvi.iItem);
						text.Replace(_T('\n'), _T(' '));
					}
				}
				break;
			case FSI_SUB_FILEINFO:
				{
					LVITEM lvi = {};
					lvi.iItem = pDispInfo->iItem; lvi.mask = LVIF_IMAGE;
					m_NdsFSI.GetItem(&lvi);
					CString h, sub;
					h.LoadString(IDS_NDSHEADERINFO);
					AfxExtractSubString(text, h, HEADERCOUNT+lvi.iImage);
					sub = m_NdsFSI.GetItemText(lvi.iItem, FSI_SUB_FILEOFFSET);
					UINT nOff = StrToIntEX(sub);
					if(nOff==-1) break;
					sub = GetSubHeader(nOff, lvi.iItem);
					UINT i=0, nSize;
					while(TRUE)
					{
						AfxExtractSubString(h, sub, i);
						nSize = StrToIntEX(h.Right(h.GetLength()-5), FALSE);
						h = h.Left(4);
						/*if(h==_T("CMAP"))
						{
						NDSCMAP *cmap = (NDSCMAP*)(m_pRom+nOff);
						h.LoadString(IDS_NDSCMAP_INFO);
						text.AppendFormat(h, cmap->nBeginCode, cmap->nEndCode, cmap->nType,
						cmap->nOffset, cmap->wChar[0], cmap->wChar[0]);
						}else*/
						if(h==_T("CWDH"))
						{
							NDSCWDH *cwdh = (NDSCWDH*)(m_Rom.GetBuf()+nOff);
							//h.LoadString(IDS_NDSCWDH_INFO);
							text.AppendFormat(h, cwdh->wCount+1, cwdh->nWDH[0][0], cwdh->nWDH[0][1], cwdh->nWDH[0][2]);
						}
						else if(h==_T("CGLP"))
						{
							NDSCGLP *cglp = (NDSCGLP*)(m_Rom.GetBuf()+nOff);
							//h.LoadString(IDS_NDSCGLP_INFO);
							text.AppendFormat(h, cglp->nWidth, cglp->nHeight, cglp->nTileSize, cglp->nBits, cglp->nTileSize*8/(cglp->nWidth*cglp->nHeight));
						}
						else if(h.IsEmpty())
							break;

						nOff += nSize;
						i++;
					}
				}break;
			}
			if(text.GetLength()>pDispInfo->cchTextMax)
			{
				text = text.Left(pDispInfo->cchTextMax-4);
				text.Append(_T("..."));
			}
			::lstrcpy(pDispInfo->pszText, text);
		}
#endif
	}
	else
	{
#if 0
		if(pDispInfo->mask & LVIF_TEXT)
		{
			switch(pDispInfo->iSubItem)
			{
			case FSI_SUB_FILEID://ID
				text.LoadString(IDS_NDSHEADER);
				break;
			case FSI_SUB_FILEOFFSET://Offset
				text.Format(_T("%08X"), m_SpecRec[FileID].nTop);
				break;
			case FSI_SUB_FILESIZE://Size
				{
					text.Format(_T("%d"), m_SpecRec[FileID].nBottom-m_SpecRec[FileID].nTop);
					int nPos=text.GetLength();while(nPos>3){nPos-=3; text.Insert(nPos, _T(','));}
				}
				break;
			case FSI_SUB_FILEINFO:
				{
					CString h;h.LoadString(IDS_NDSHEADERINFO);
					AfxExtractSubString(text, h, FileID);
				}
				break;
			}
			::lstrcpy(pDispInfo->pszText, text);
		}
#endif
	}

	*pResult = 0;
}

void CWQSG_NDSDlg::OnLoadFSI(void)
{
	m_NdsFSI.SetRedraw(FALSE);

	m_NdsFSI.DeleteAllItems();

	if( m_Rom.IsOpen() )
	{
		SNdsFindHandle* pHandle = m_Rom.FindFile( m_strPath );

		if( pHandle )
		{
			SNdsFindData data;

			while(true)
			{
				if( !m_Rom.FindNextFile( data , pHandle /*, true*/ ) )
					ASSERT(0);

				if( data.m_bEmpty )
					break;

				CString str;
				str = data.m_szName;

				const int iIndex = m_NdsFSI.InsertItem( m_NdsFSI.GetItemCount() , str );
				if( iIndex == -1 )
				{
					ASSERT(iIndex != -1);
					continue;
				}

				const SNdsFindData*const pData = new SNdsFindData( data );
				m_NdsFSI.SetItemData( iIndex , (DWORD_PTR)pData );

				if( pData->m_bDir )
				{
					const SNdsFindData::SDir& dir = pData->m_Dir;
					str.Format( L"Dir(%d)" , dir.m_uDir_ID );
					m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILEID , str );
					//m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILEOFFSET , str );
					//m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILESIZE , str );
					//m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILEHEADER , str );
					//m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILEINFO , str );
				}
				else
				{
					const SNdsFindData::SFile& file = pData->m_File;
					str.Format( L"%d" , file.m_uFileID_InRom );
					m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILEID , str );

					str.Format( L"%08X" , file.m_uRomOffset );
					m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILEOFFSET , str );

					str.Format( L"%d" , file.m_uSize );
					m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILESIZE , str );

					//str.Format( L"%d" , );
					str = "";
					m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILEHEADER , str );

					//str.Format( L"%d" , );
					str = "";
					m_NdsFSI.SetItemText( iIndex , FSI_SUB_FILEINFO , str );
				}
			}

			m_Rom.FindClose( pHandle );
		}

	}

	m_NdsFSI.SetRedraw();
#if 0
	OnUpdateNds();

	/*POSITION pos = m_NdsFSI.GetFirstSelectedItemPosition();
	int nSel = -1;
	if(pos)
	nSel = m_NdsFSI.GetNextSelectedItem(pos);*/
	int nSel = GetCurSel();

	m_NdsFSI.DeleteAllItems();

	if( m_pHeader )
	{
		m_nOverlayFiles9 = m_pHeader->Arm9_Overlay_Size / sizeof(OVERLAYENTRY);
		m_nOverlayFiles7 = m_pHeader->Arm7_Overlay_Size / sizeof(OVERLAYENTRY);

		m_nRomFitSize = 0;	int nItem = 0;

		for( ; nItem<HEADERCOUNT ; nItem++ )
			m_NdsFSI.InsertItem(nItem, m_SpecRec[nItem].FileName, FSI_ICON_NDS);

		CString OverlayFiles;
		m_nOverlayFileSize = 0; 
		OVERLAYENTRY* OverlayEntry = (OVERLAYENTRY*)(m_Rom.GetBuf()+m_pHeader->Arm9_Overlay_Offset);
		NDSFILEREC* pFileRec;
		for( UINT i= 0 ; i < m_nOverlayFiles9 ; i++ )
		{
			OverlayFiles.Format(OVERLAY_FMT, 9, i);

			pFileRec = (NDSFILEREC*)(m_Rom.GetBuf()+m_pHeader->Fat_Offset+(OverlayEntry->file_id<<3));
			m_nOverlayFileSize += pFileRec->bottom-pFileRec->top;

			m_NdsFSI.InsertItem(LVIF_TEXT|LVIF_PARAM|LVIF_IMAGE, nItem,
				OverlayFiles, 0, 0, FSI_ICON_OT, MAKEPARAM(OverlayEntry->file_id, nItem));
			nItem++;
			OverlayEntry++;
		}
		OverlayEntry = (OVERLAYENTRY*)(m_Rom.GetBuf()+m_pHeader->Arm7_Overlay_Offset);
		for( UINT i = 0 ; i < m_nOverlayFiles7 ; i++ )
		{
			OverlayFiles.Format(OVERLAY_FMT, 7, i);

			pFileRec = (NDSFILEREC*)(m_Rom.GetBuf()+m_pHeader->Fat_Offset+(OverlayEntry->file_id<<3));
			m_nOverlayFileSize += pFileRec->bottom-pFileRec->top;

			m_NdsFSI.InsertItem( LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE , nItem ,
				OverlayFiles , 0 , 0 , FSI_ICON_OT , MAKEPARAM(OverlayEntry->file_id, nItem) );

			nItem++;
			OverlayEntry++;
		}

		OverlayFiles  = _T("/");
		ExtractDirectory(OverlayFiles);

		m_nRomFitSize +=
			sizeof(NDSHEADER)							//ndsheader
			+m_pHeader->Arm9_Size						//arm9
			+m_pHeader->Arm7_Size						//arm7
			+m_pHeader->Arm9_Overlay_Size				//arm9ovltable
			+m_pHeader->Arm7_Overlay_Size				//arm7ovltable
			+0x840									//banner
			+m_nOverlayFileSize;					//OverlayFileSize

		LVCOLUMN col = {};
		col.mask = LVCF_TEXT; col.iSubItem = 2; m_NdsFSI.GetColumn(3, &col);
		OverlayFiles.Format(_T("��С(%d)"), m_nRomFitSize);
		col.pszText = (LPTSTR)(LPCTSTR)OverlayFiles; col.mask = LVCF_TEXT;
		m_NdsFSI.SetColumn( 3 , &col );
	}

	m_NdsFSI.SetRedraw();

	if( nSel != -1 )
	{
		m_NdsFSI.SetItemState(nSel, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		m_NdsFSI.SendMessage(WM_KEYDOWN, VK_LEFT);
	}
#endif
}


void CWQSG_NDSDlg::OnRomopen()
{
	// TODO: �ڴ�����������������
	UpdateData();

	CFileDialog dlg( TRUE );
	dlg.m_ofn.lpstrFilter = L"*.nds\0*.nds\0\0";

	if( dlg.DoModal() != IDOK )
		return;

	//m_Rom.Close();
	OnRomclose();

	m_strPath = "";

	if( !m_Rom.Open( dlg.GetPathName() ) )
	{
		MessageBox( L"" );
	}

	m_NdsFSI.EnableWindow( TRUE );
	OnLoadFSI();

	return;
}

void CWQSG_NDSDlg::OnLvnItemActivateFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	*pResult = 0;

	const SNdsFindData* pData = (SNdsFindData*)m_NdsFSI.GetItemData( pNMIA->iItem );
	if( !pData || !pData->m_bDir )
		return;

	m_strPath.AppendFormat( "/%s" , pData->m_szName );

	OnLoadFSI();
}

void CWQSG_NDSDlg::OnLvnDeleteitemFsilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	*pResult = 0;

	const SNdsFindData* pData = (SNdsFindData*)m_NdsFSI.GetItemData( pNMLV->iItem );
	if( pData )
		delete pData;
}

BOOL CWQSG_NDSDlg::DestroyWindow()
{
	// TODO: �ڴ�����ר�ô����/����û���
	OnRomclose();

	return CDialog::DestroyWindow();
}

void CWQSG_NDSDlg::OnRomclose()
{
	// TODO: �ڴ�����������������
	m_Rom.Close();
	m_strPath = "";

	m_NdsFSI.EnableWindow( FALSE );

	OnLoadFSI();
}

void CWQSG_NDSDlg::OnBnClickedButtonUpdir()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	if( m_strPath == "" )
		return;

	int iPos = m_strPath.ReverseFind( '/' );
	if( -1 == iPos )
		m_strPath = "";
	else
		m_strPath = m_strPath.Left( iPos );

	OnLoadFSI();
}


BOOL CWQSG_NDSDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ�����ר�ô����/����û���
	if( WM_DROPFILES == pMsg->message )
	{
		const HDROP hDrop = (HDROP)pMsg->wParam;
		if( pMsg->hwnd == m_NdsFSI.m_hWnd )
		{
			if( m_Rom.IsCanWrite() )
			{
				WCHAR strPathName[MAX_PATH*2];

				const int fileCount = DragQueryFile( hDrop , (UINT)-1 , NULL , 0 );

				for( int i = 0 ; (i>=0) && (i<fileCount) && ( DragQueryFile( hDrop , i , strPathName , MAX_PATH*2 ) != (UINT)-1 ) ; ++i )
				{
					if( WQSG_IsDir( strPathName ) )
					{
						WDir( strPathName , m_strPath );
					}
					else if( WQSG_IsFile( strPathName ) )
					{
						SNdsFindData dirData;
						if( m_Rom.GetPath( dirData , m_strPath ) )
						{
							if( !WFile( strPathName , dirData , "" ) )
							{
								//MessageBox( m_Rom.GetErrStr() );
								//break;
							}
						}
					}
				}
				OnLoadFSI();
				//UpDataLbaData();
			}
		}
		DragFinish( hDrop );
	}

	return CDialog::PreTranslateMessage(pMsg);
}

bool CWQSG_NDSDlg::WFile( CString a_strPathFile , const SNdsFindData& a_DirData , CStringA a_strFile )
{
	CWQSG_File fp;
	if( !fp.OpenFile( a_strPathFile.GetString() , 1 , 4 ) )
		return false;

	if( a_strFile.GetLength() == 0 )
	{
		const int pos = a_strPathFile.ReverseFind( L'\\' );
		if( pos == -1 )
		{
			ASSERT(pos != -1);
			return false;
		}

		a_strFile = a_strPathFile.Mid( pos + 1 );
	}

	return m_Rom.ImportFile( a_DirData , fp , a_strFile );
}

bool CWQSG_NDSDlg::WDir( CString a_strPathFile , const CStringA& a_strDirPath )
{
	SNdsFindData dirData;
	if( m_Rom.GetPath( dirData , a_strDirPath ) )
		return false;

	if( a_strPathFile.Right(1) != L'\\' )
		a_strPathFile += L'\\';

	WIN32_FIND_DATAW data;

	const HANDLE handle = FindFirstFile( a_strPathFile + L'*' , &data );
	if( handle == INVALID_HANDLE_VALUE )
		return true;

	do 
	{
		if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			CStringA strNameA;
			strNameA = data.cFileName;

			WDir( a_strPathFile + data.cFileName , a_strDirPath + "/" + strNameA );
		}
		else
		{
			CStringA strNameA;
			strNameA = data.cFileName;

			WFile( a_strPathFile + data.cFileName , dirData , strNameA );
		}
	}
	while ( FindNextFile( handle , &data ));

	FindClose(handle);

	return true;
}