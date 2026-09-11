#include "Kameda/PrintProgressive.h"
#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRFileLoader.h"
#include "JSystem/JUtility/JUTAssert.h"
#include "Kameda/ReadPrintMessage.h"
#include "Osako/FontMgr.h"
#include "Osako/ResMgr.h"
#include "Osako/system.h"

PrintProgressive::PrintProgressive(JKRHeap *heap)
{
    mOrthoGraph = System::getJ2DOrtho();

    mArchive = JKRMountArchive(ResMgr::getPtr(ResMgr::mcArcSystem, "Screen/progressive.arc"),
                               heap,
                               JKRArchive::MOUNT_DIRECTION_HEAD);

    mScreenLogo = new (heap, 0) J2DScreen();
    mScreenLogo->J2DScreen::set("NintendoLogo.blo", 0x40000, mArchive);

    mScreenProgressive = new (heap, 0) J2DScreen();
    mScreenProgressive->J2DScreen::set("progressive.blo", 0x40000, mArchive);

    mTransform = (J2DAnmTransform *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("Progressive.bck", mArchive));
    mScreenProgressive->setAnimation(mTransform);

    {
        J2DPicture *pane = (J2DPicture *)mScreenProgressive->search('n_an');
#line 49
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPane = pane;
    }

    mTextBox = (J2DTextBox *)mScreenProgressive->search('T_tx1');
    mTextBox->setFont(FontMgr::mspResFont);
    mTextBox->getFontSize(mFontSize);

    mTextBox2 = (J2DTextBox *)mScreenProgressive->search('T_yes');
    mTextBox2->setFont(FontMgr::mspResFont);
    mTextBox2->getFontSize(mFontSize2);

    mTextBox3 = (J2DTextBox *)mScreenProgressive->search('T_no');
    mTextBox3->setFont(FontMgr::mspResFont);
    mTextBox3->getFontSize(mFontSize3);

    mTextBox4 = (J2DTextBox *)mScreenProgressive->search('Num1');
    mTextBox4->setFont(FontMgr::mspResFont);

    {
        J2DPicture *pane = (J2DPicture *)mScreenProgressive->search('lyes');
#line 66
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPane2 = pane;
    }
    mPane2->setAnimation(mTransform);

    {
        J2DPicture *pane = (J2DPicture *)mScreenProgressive->search('lno');
#line 72
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPane3 = pane;
    }
    mPane3->setAnimation(mTransform);

    {
        J2DPicture *pane = (J2DPicture *)mScreenLogo->search('m_nin1');
#line 78
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPane4 = pane;
    }

    {
        J2DPicture *pane = (J2DPicture *)mScreenLogo->search('Dlby0');
#line 83
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPane5 = pane;
    }

    void *resource = JKRGetNameResource("Progressive.bpk", mArchive);
    for (int i = 0; i < 4; ++i)
    {
        mColors[i] = (J2DAnmColor *)J2DAnmLoaderDataBase::load(resource);
        mColors[i]->searchUpdateMaterialID(mScreenProgressive);
    }

    mTextBox2->setAnimation(mColors[0]);
    mTextBox3->setAnimation(mColors[1]);
    mPane2->setAnimation(mColors[2]);
    mPane3->setAnimation(mColors[3]);

    mSTRKey = (J2DAnmTextureSRTKey *)J2DAnmLoaderDataBase::load(
        JKRFileLoader::getGlbResource("Progressive.btk", mArchive));
    mScreenProgressive->setAnimation(mSTRKey);

    mColorRes = ResMgr::getPtr(ResMgr::mcColor);
    mProgressiveScanRes = ResMgr::getPtr(ResMgr::mcProgressiveScan);
    mYesNoRes = ResMgr::getPtr(ResMgr::mcYesNo);

    JUTColor color =
        ReadPrintMessage::getColor(mColorRes,
                                   ReadPrintMessage::getDefaultColor(mProgressiveScanRes));
    mTextBox->setFontColor(color, color);

    char buffer[512];
    for (int i = 0; i < 512; ++i)
    {
        buffer[i] = ' ';
    }
    buffer[511] = '\0';
    mTextBox->setString(buffer);
    buffer[15] = '\0';
    mTextBox2->setString(buffer);
    mTextBox3->setString(buffer);
    buffer[1] = '\0';
    mTextBox4->setString(buffer);
}

void PrintProgressive::init(PrintProgressive::MessageID) {}

void PrintProgressive::draw()
{
    mOrthoGraph->setPort();

    switch (mMsgId)
    {
    case mcShow_Logo:
    case mcShow_Set:
        mScreenLogo->draw(0.0f, 0.0f, mOrthoGraph);
        break;
    default:
        mScreenProgressive->draw(0.0f, 0.0f, mOrthoGraph);
    }
}

void PrintProgressive::calc(int) {}

bool PrintProgressive::isMessage()
{
    bool result;

    switch (mMsgId)
    {
    case mcShow_Dialog:
    case mcSet_Option1:
    case mcSet_Option2:
        result = true;
        break;
    case mcShow_Logo:
    case mcShow_Set:
        result = false;
        break;
    default:
#line 497
        JUT_ASSERT(0);
    }

    return result;
}

bool PrintProgressive::isChoice()
{
    bool result;

    switch (mMsgId)
    {
    case mcShow_Dialog:
        result = true;
        break;
    case mcSet_Option1:
    case mcSet_Option2:
    case mcShow_Logo:
    case mcShow_Set:
        result = false;
        break;
    default:
#line 524
        JUT_ASSERT(0);
    }

    return result;
}

s32 PrintProgressive::getCounter()
{
    s32 result = -1;
    switch (mMsgId)
    {
    case mcShow_Dialog:
        if (_60 != 0)
        {
            result = _60 / 0x3c + 1;
        }
        else
        {

            result = 0;
        }
        break;
    default:
        break;
    }

    return result;
}

#include "JSystem/JAudio/JASFakeMatch.h"  // For static initializer
