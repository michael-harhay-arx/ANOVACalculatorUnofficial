//==============================================================================
//
// Title:		ANOVACalculator
// Purpose:		A short description of the application.
//
// Created on:	2025-07-09 at 11:42:42 AM by .
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include "toolbox.h"
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>

#include "ANOVACalculator.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle = 0;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/***************************************************************************//*!
* \brief Main entry point
*******************************************************************************/
int main (int argc, char *argv[])
{
	int error = 0;
	
	/* initialize and load resources */
	nullChk (InitCVIRTE (0, argv, 0));
	errChk (panelHandle = LoadPanel (0, "MainPanel.uir", MAINPANEL));
	
	/* display the panel and run the user interface */
	errChk (DisplayPanel (panelHandle));
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (panelHandle > 0)
		DiscardPanel (panelHandle);
	return 0;
}

/***************************************************************************//*!
* \brief Parse through CSV table selections and prepare data for calculations
*
* \param [in] xx			asdfasdfasdf
*******************************************************************************/
int ParseCSVSelection ()
{
	
	return 0;
}

/***************************************************************************//*!
* \brief Perform ANOVA calculations
*******************************************************************************/
int ComputeANOVA ()
{
	return 0;
}

/***************************************************************************//*!
* \brief Create new ANOVANode
*
* \param [in] Key				A specific element from a factor (e.g., Factor "SN"
*								contains the labels 1001, 1002, 1003... etc.)
*******************************************************************************/
ANOVANode *CreateANOVANode (IN char Key[32])
{
	ANOVANode *node = malloc (sizeof (ANOVANode));
	strcpy (node->key, Key);
	node->numValues = 0;
	node->values = NULL;
	node->numChildren = 0;
	node->children = NULL;
	
	return node;
}

/***************************************************************************//*!
* \brief Add a value to a node's list of values
*
* \param [in] Node				The target node
* \param [in] Value				The value to be added
*******************************************************************************/
void AddNodeValue (ANOVANode *Node, IN double Value)
{
	Node->numValues++;
	Node->values = realloc (Node->values, sizeof (double) * (Node->numValues));
	Node->values[Node->numValues - 1] = Value;
}

/***************************************************************************//*!
* \brief Assign a node to be a child of another node
*
* \param [in] ParentNode		The target parent node
* \param [in] ChildNode			The target child node
*******************************************************************************/
void AddChildNode (ANOVANode *ParentNode, ANOVANode *ChildNode)
{
	ParentNode->numChildren++;
	ParentNode->children = realloc (ParentNode->children, sizeof (ANOVANode *) * (ParentNode->numChildren));
	ParentNode->children[ParentNode->numChildren - 1] = ChildNode;
}
