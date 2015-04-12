/* 
 * File:   Alert.cpp
 * Author: Joshua Johannson
 *
 * 
 * --------------------------------------
 * Alert CLASS
 * show alert message
 * --------------------------------------
 */

#include "Alert.h"


// -- CREATE OBJECT ----------------
Alert::Alert(Box *rootView)
{
    this->rootView  = rootView;
    this->visible   = false;

    // reference text size
    #ifdef pl_andr
    refTextSize = 50;
    #else
    refTextSize = 30;
    #endif

    init();
}

// -- INIT -------------------------
void Alert::init()
{
    // style
    StyleRule *styAll  = new StyleRule(".alert");
    StyleRule *styBox  = new StyleRule("#alertBox");
    StyleRule *styHead = new StyleRule("#alertHead");
    StyleRule *styEBox = new StyleRule("#alertEntryBox");
    StyleRule *styEntr = new StyleRule("#alertEntry");
    StyleRule *styButt = new StyleRule("#alertButton");

    styAll->textSize->set(refTextSize);
    styAll->textColor->set("000000DD");
    styAll->width->setPercent(100);

    styBox->position->set(UI_ATTR_POSITION_ABSOLUTE);
    styBox->top->set(200);
    styBox->left->set(40);
    styBox->right->set(40);
    styBox->backgroundColor->set("FFFFFFFF"); // white

    styHead->backgroundColor->set("33B5E5");  // blue
    styHead->textColor->set("FFFFFFFF");      // white
    styHead->textSize->set(refTextSize*1.3);
    styHead->paddingLeft->set(5);
    styHead->paddingRight->set(5);
    styHead->paddingTop->set(10);
    styHead->paddingBottom->set(10);

    styButt->textSize->set(refTextSize*1.1);
    styButt->top->set(15);
    styButt->paddingLeft->set(5);
    styButt->paddingRight->set(5);
    styButt->paddingTop->set(5);
    styButt->paddingBottom->set(10);
    styButt->backgroundColor->set("33B5E5"); // blue
    styButt->textColor->set("FFFFFF");

    styEBox->paddingLeft->set(10);
    styEBox->paddingRight->set(10);
    styEBox->paddingTop->set(10);
    styEBox->paddingBottom->set(10);

    styEntr->bottom->set(8);

    Style::addRule(styAll);
    Style::addRule(styBox);
    Style::addRule(styHead);
    Style::addRule(styButt);
    Style::addRule(styEBox);
    Style::addRule(styEntr);


    // views
    rootBox     = new Box ("#alertBox",     ".alert");
    headline    = new Text("#alertHead",    ".alert");
    entryBox    = new Box ("#alertEntryBox",".alert");
    btOk        = new Text("#alertButton",  ".alert");

    headline->text("Achtung");
    btOk->text("ok");


    // on ok button click
    btOk->onTouchDown([&](View *v, Point relativeSelf, Point relativeParent, Point absolute)
    {
        // hide self
        hide();
    });


    // add
    rootBox->addChild(headline);
    rootBox->addChild(entryBox);
    rootBox->addChild(btOk);
}

// -- SHOW -------------------------
void Alert::show(string msg)
{
    // add msg entry
    Text *entry = new Text("", ".alert");
    entry->text(msg);
    entryBox->addChild(entry);

    // show if not already
    if (!visible)
    {
        rootView->addChild(rootBox);
        visible = true;
    }
}

void Alert::show(string msg, View *button)
{
    // add entry
    Box  *entry = new Box("", ".alert");
    entryBox->addChild(entry);

    Text *text  = new Text("", ".alert");
    text->text(msg);
    text->style->width->setPercent(68);
    button->class_ = ".alert";
    button->id     = "#alertButton";
    button->style->width->setPercent(30);
    button->style->top->set(0);
    button->getStyle();
    entry->addChild(text);
    entry->addChild(button);

    // show if not already
    if (!visible)
    {
        rootView->addChild(rootBox);
        visible = true;
    }
}

// -- HIDE -------------------------
void Alert::hide()
{
    // hide
    rootView->removeChild(rootBox);
    visible = false;

    // remove each entry
    for (auto it = entryBox->children.begin(); it != entryBox->children.end(); it++)
    {
        // cout << "delete view: " << (*it)->id << endl;
        free(*it);
        entryBox->children.erase(it++);
    }
}