/*
 *      $Id: createmenus.c,v 1.3 1997-06-23 21:06:22 dbrown Exp $
 */
/************************************************************************
*									*
*			     Copyright (C)  1996			*
*	     University Corporation for Atmospheric Research		*
*			     All Rights Reserved			*
*									*
************************************************************************/
/*
 *	File:		createmenus.c
 *
 *	Author:		David I. Brown
 *			National Center for Atmospheric Research
 *			PO 3000, Boulder, Colorado
 *
 *	Date:		Fri Jun  6 17:26:33 MDT 1997
 *
 *	Description:	
 */

#include <ncarg/ngo/createmenusP.h>
#include <ncarg/ngo/xutil.h>
#include <ncarg/ngo/sort.h>

#include <Xm/Xm.h>
#include <Xm/Protocols.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/MenuShell.h>
#include <Xm/MessageB.h>
#include <Xm/TextF.h>
#include  <Xm/Form.h>
#include  <Xm/LabelG.h>

static NgDataSinkRec DataSinks[] = {
	{NgLineContour, ngLINECONTOUR,"cn_obj","contourPlotClass",
         3,{ 2, 1, 1 },{ "scalar field", "x coord", "y coord" },
         {NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK},{ 1, 2 }
        },
	{NgFillContour, ngFILLCONTOUR,"cn_obj","contourPlotClass",
         3,{ 2, 1, 1 },{ "scalar field", "x coord", "y coord" },
         {NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK},{ 1, 2 }
        },
	{NgRasterContour, ngRASTERCONTOUR,"cn_obj","contourPlotClass",
         3,{ 2, 1, 1 },{ "scalar field", "x coord", "y coord" },
         {NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK},{ 1, 2 }
        },
	{NgStreamline, ngSTREAMLINE,"st_obj","streamlinePlotClass",
         4,{ 2, 2, 1, 1 },
         { "vector field u", "vector field v", "x coord", "y coord" },
         {NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK},{ 2,3 }
        },
	{NgLineVector, ngLINEVECTOR,"vc_obj","vectorPlotClass",
         5,{ 2, 2, 2, 1, 1 }, 
         { "vector field u", "vector field v", "scalar field",
           "x coord", "y coord" },
         {NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK},
         { 3, 4 }
        },
	{NgFillVector, ngFILLVECTOR,"vc_obj","vectorPlotClass",
         5,{ 2, 2, 2, 1, 1 }, 
         { "vector field u", "vector field v", "scalar field",
           "x coord", "y coord" },
         {NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK},
         { 3, 4 }
        },
	{NgLineXy, ngLINEXY,"xy_obj","xyPlotClass",
         2,{ -2, -2 } , { "x array", "y array" },
         {NrmNULLQUARK,NrmNULLQUARK},{ 2,3 }
        },
	{NgScatterXy, ngSCATTERXY,"xy_obj","xyPlotClass",
         2,{ -2, -2 } , { "x array", "y array" },
         {NrmNULLQUARK,NrmNULLQUARK},{ 2, 3 }
        },
	{NgCoordArray, ngCOORDARRAY,"ca_obj","coordArraysClass",
         2,{ -2, -2 } , { "x array", "y array" },
         {NrmNULLQUARK,NrmNULLQUARK},{ 0,1 }
        },
	{NgScalarField, ngSCALARFIELD,"sf_obj","scalarFieldClass",
         3,{ 2, 1, 1 } ,  { "scalar field", "x coord", "y coord" },
         {NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK},{ 1, 2 }
        },
	{NgVectorField, ngVECTORFIELD,"vc_obj","vectorFieldClass",
         4,{ 2, 2, 1, 1 } ,
         { "vector field u", "vector field v", "x coord", "y coord" },
         {NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK,NrmNULLQUARK},{ 2, 3 }
        },
        {NgNclVariable, ngNCLVARIABLE,"ncl_var","none",
         1,{ -1 },{"var"},{ 1, 1 }
        }
};

static void CopyShapedVar
(
        CreateMenusRec	*priv,
        NhlString	varname
        )
{
        char buf[256];
	NgCreateMenus	*pub = &priv->public;
        int i;

        if (pub->qsymbol)
                sprintf(buf,"%s = %s->%s(",NgNclGetSymName(varname,False),
                        NrmQuarkToString(pub->qsymbol),
                        NrmQuarkToString(pub->vinfo->name));
        else
                sprintf(buf,"%s = %s(",NgNclGetSymName(varname,False),
                        NrmQuarkToString(pub->qsymbol),
                        NrmQuarkToString(pub->vinfo->name));
        for (i = 0; i < pub->vinfo->n_dims; i++) 
		sprintf(&buf[strlen(buf)],"%d:%d:%d,",pub->start[i],
			pub->finish[i],pub->stride[i]);
        
        	/* backup 1 to remove last comma */
	sprintf(&buf[strlen(buf)-1],")\n");

	(void)NgNclSubmitBlock(priv->nsid,buf);

        return;
}

static void CreateCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	CreateMenusRec	*priv = (CreateMenusRec	*)udata;
	NgCreateMenus	*pub = &priv->public;
        NgMenuRec	*plot = &priv->plot;
        NgDataSinkRec	*sink;
        NrmQuark	qname;
        char		*vartext;
        NgPageId	page_id;
        NgHluPage	*hlu_page;
        
        printf("in create cb\n");
        XtVaGetValues(w,
                      XmNuserData,&sink,
                      NULL);
        XtVaGetValues(priv->dialog_text,
                      XmNvalue,&vartext,
                      NULL);
            /* need to qualify text string, and warn user if it's already
               a symbol */
        if (sink->type == ngNCLVARIABLE) {
                CopyShapedVar(priv,vartext);
                return;
        }
        else {
                char buf[256];
                NhlString varname = NgNclGetSymName(vartext,False);
                
                    /* create the NCL graphic variable using this name now
                       in order that it won't be "stolen" before the hlu
                       object actually gets created */
                
                sprintf(buf,"%s = new(1,graphic)\n",varname);
                (void)NgNclSubmitBlock(priv->nsid,buf);

                qname = NrmStringToQuark(vartext);
                page_id = NgOpenPage(priv->go->base.id,_brHLUVAR,&qname,1);
                if (page_id <= NgNoPage) {
                        printf("unable to open hlu page\n");
                        return;
                }
                hlu_page = (NgHluPage *)NgPageData(priv->go->base.id,page_id);
                if (! hlu_page) {
                        printf("unable to get public page data\n");
                        return;
                }
                hlu_page->class_name = sink->class_name;
                hlu_page->data_info = sink;

                if (NgUpdatePage(priv->go->base.id,page_id) < NhlWARNING) {
                        printf("error updating hlu page\n");
                        return;
                }
                
                (*pub->output_notify)(pub->pdata,page_id);
        }
        
        return;
}

static void CreateDialog
(
       CreateMenusRec	*priv,
       NgDataSinkRec 	*sink
       )
{
	Arg	args[50];
	int	nargs;
	NgCreateMenus	*pub = &priv->public;
	char    buf[128] = "";
        XmString xmname;
        Widget  form,label,help;
        
        if (sink->type == ngNCLVARIABLE)
                sprintf(buf,"Create Ncl Variable",sink->name);
        else
                sprintf(buf,"Create %sPlot",sink->name);
        
        xmname = NgXAppCreateXmString(priv->go->go.appmgr,buf);
        printf("%s\n",buf);

        nargs = 0;
	XtSetArg(args[nargs],XmNdialogTitle,xmname);nargs++;
	XtSetArg(args[nargs],XmNuserData,sink);nargs++;
        if (! priv->create_dialog) {
                priv->create_dialog = XmCreateMessageDialog
                        (pub->menubar,"CreateDialog",args,nargs);
                help = XmMessageBoxGetChild
                        (priv->create_dialog,XmDIALOG_HELP_BUTTON);
                XtUnmanageChild(help);
		XtAddCallback(priv->create_dialog,
			      XmNokCallback,CreateCB,priv);
		form = XtVaCreateManagedWidget
                        ("form",xmFormWidgetClass,
                         priv->create_dialog,
                         NULL);
                label = XtVaCreateManagedWidget
                        ("Name",xmLabelGadgetClass,
                         form,
                         XmNrightAttachment,XmATTACH_NONE,
                         NULL);
                priv->dialog_text = XtVaCreateManagedWidget
                        ("dialog",xmTextFieldWidgetClass,
                         form,
                         XmNleftAttachment,XmATTACH_WIDGET,
                         XmNleftWidget,label,
                         XmNvalue,NgNclGetSymName(sink->def_name,True),
                         XmNresizeWidth,True,
                         NULL);
        }
	else {
		XtSetValues(priv->create_dialog,args,nargs);
                XtVaSetValues(priv->dialog_text,
                              XmNvalue,NgNclGetSymName(sink->def_name,True),
                              NULL);
	}
	XmStringFree(xmname);
        XtManageChild(priv->create_dialog);
        
        return;
        
}

static void CreateDialogCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	CreateMenusRec	*priv = (CreateMenusRec	*)udata;
	NgCreateMenus	*pub = &priv->public;
        NgMenuRec	*plot = &priv->plot;
        NgDataSinkRec	*sink;

        printf("in plot create cb\n");

        XtVaGetValues(w,
                      XmNuserData,&sink,
                      NULL);
        CreateDialog(priv,sink);
        return;
        
}

static void PlotMenuCB 
(
	Widget		w,
	XtPointer	udata,
	XtPointer	cb_data
)
{
	CreateMenusRec	*priv = (CreateMenusRec	*)udata;
	NgCreateMenus	*pub = &priv->public;
        NgMenuRec	*plot = &priv->plot;
        int		i;

        printf("in plot menu cb\n");

        if (plot->count == 0) {
                plot->count = ngSCATTERXY + 1;
                plot->buttons = NhlMalloc(plot->count * sizeof(Widget));
                for (i = 0; i < plot->count; i++) {
                        plot->buttons[i] = XtVaCreateManagedWidget
                                (DataSinks[i].name,
                                 xmCascadeButtonGadgetClass,
                                 plot->menu,
                                 XmNuserData,&DataSinks[i],
                                 NULL);
                        XtAddCallback(plot->buttons[i],
                                      XmNactivateCallback,CreateDialogCB,
                                      priv);
                }
        }
        return;
}
NhlErrorTypes NgUpdateCreateMenus
(
        NgCreateMenus		*create_menus
        )
{
	NgCreateMenus	*pub = create_menus;
	CreateMenusRec	*priv = (CreateMenusRec	*)pub;
}

NgCreateMenus *
NgCreateCreateMenus
(
        NgGO            go,
        Widget		parent
)
{
	CreateMenusRec	*priv;
	NgCreateMenus	*pub;
        Widget		menush;

        priv = NhlMalloc(sizeof(CreateMenusRec));
        priv->go = go;
        priv->create_dialog = NULL;
	pub = &priv->public;
        
	NhlVAGetValues(priv->go->go.appmgr,
		NgNappNclState,	&priv->nsid,
		NULL);
        
        priv->plot.count = priv->var.count = priv->data.count = 0;
        priv->plot.alloced = priv->var.alloced = priv->data.alloced = 0;
        
        pub->menubar =  XtVaCreateManagedWidget
                ("CreateMenu",xmRowColumnWidgetClass,
                 parent,
                 XmNrowColumnType,      XmMENU_BAR,
                 NULL);

	menush = XtVaCreatePopupShell
                ("override_sh",xmMenuShellWidgetClass,
                 pub->menubar,
                 XmNwidth,		5,
                 XmNheight,		5,
                 XmNallowShellResize,	True,
                 XtNoverrideRedirect,	True,
                 NULL);
        
        priv->plot.menu =  XtVaCreateWidget
                ("Plot",xmRowColumnWidgetClass,menush,
                 XmNrowColumnType,	XmMENU_PULLDOWN,
                 NULL);
	XtAddCallback(priv->plot.menu,
		      XmNmapCallback,PlotMenuCB,priv);

	pub->plot_mbutton = 
                XtVaCreateManagedWidget
                ("Plot",xmCascadeButtonGadgetClass,
                 pub->menubar,
                 XmNsubMenuId,	priv->plot.menu,
                 NULL);

        priv->var.menu = XtVaCreateWidget
                ("Variable",xmRowColumnWidgetClass,menush,
                 XmNrowColumnType,	XmMENU_PULLDOWN,
                 XmNuserData,&DataSinks[ngNCLVARIABLE],
                 NULL);
	XtAddCallback(priv->var.menu,
		      XmNmapCallback,CreateDialogCB,priv);

	pub->var_mbutton = 
                XtVaCreateManagedWidget
                ("Variable",xmCascadeButtonGadgetClass,
                 pub->menubar,
                 XmNuserData,&DataSinks[ngNCLVARIABLE],
                 NULL);
        XtAddCallback(pub->var_mbutton,
                      XmNactivateCallback,CreateDialogCB,
                      priv);
        
	XtManageChild(priv->plot.menu);
	XtManageChild(priv->var.menu);

        return pub;
        
}

void NgDestroyCreateMenus
(
        NgCreateMenus		*create_menus
        )
{
	NgCreateMenus	*pub = create_menus;
	CreateMenusRec	*priv = (CreateMenusRec	*)pub;

        if (priv->plot.count)
                NhlFree(priv->plot.buttons);
        
        NhlFree(priv);

        return;
}
