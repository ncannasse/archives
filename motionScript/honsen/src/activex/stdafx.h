// stdafx.h : Fichier Include pour les fichiers Include syst�me standard,
// ou les fichiers Include sp�cifiques aux projets qui sont utilis�s fr�quemment,
// et sont rarement modifi�s

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclure les en-t�tes Windows rarement utilis�s
#endif

// Modifiez les d�finitions suivantes si vous devez cibler une plate-forme avant celles sp�cifi�es ci-dessous.
// Reportez-vous � MSDN pour obtenir les derni�res informations sur les valeurs correspondantes pour les diff�rentes plates-formes.
#ifndef WINVER				// Autorise l'utilisation des fonctionnalit�s sp�cifiques � Windows�95 et Windows NT�4 ou version ult�rieure.
#define WINVER 0x0400		// Attribuez la valeur appropri�e � cet �l�ment pour cibler Windows�98 et Windows�2000 ou version ult�rieure.
#endif

#ifndef _WIN32_WINNT		// Autorise l'utilisation des fonctionnalit�s sp�cifiques � Windows NT�4 ou version ult�rieure.
#define _WIN32_WINNT 0x0400	// Attribuez la valeur appropri�e � cet �l�ment pour cibler Windows�2000 ou version ult�rieure.
#endif						

#ifndef _WIN32_WINDOWS		// Autorise l'utilisation des fonctionnalit�s sp�cifiques � Windows�98 ou version ult�rieure.
#define _WIN32_WINDOWS 0x0410 // Attribuez la valeur appropri�e � cet �l�ment pour cibler Windows�Me ou version ult�rieure.
#endif

#ifndef _WIN32_IE			// Autorise l'utilisation des fonctionnalit�s sp�cifiques � IE�4.0 ou version ult�rieure.
#define _WIN32_IE 0x0400	// Attribuez la valeur appropri�e � cet �l�ment pour cibler IE�5.0 ou version ult�rieure.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// certains constructeurs CString seront explicites

#include <afxctl.h>         // Prise en charge MFC pour les contr�les ActiveX
#include <afxext.h>         // Extensions MFC
#include <afxdtctl.h>		// Prise en charge MFC pour les contr�les communs Internet Explorer�4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Prise en charge des MFC pour les contr�les communs Windows
#endif // _AFX_NO_AFXCMN_SUPPORT

// Supprimez les deux includes ci-dessous si vous souhaitez utiliser les
//  classes de base de donn�es MFC
#include <afxdb.h>			// Classes de base de donn�es MFC
#include <afxdao.h>			// Classes de base de donn�es DAO MFC

