#include "Kameda/PrintProgressive.h"

#include "Osako/kartPad.h"
#include "string.h"

#include "Inagaki/GameAudioMain.h"
#include "JSystem/J2D/J2DAnmLoader.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/JAudio/JASFakeMatch.h"  // For static initializer
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

    mTextBoxYes = (J2DTextBox *)mScreenProgressive->search('T_yes');
    mTextBoxYes->setFont(FontMgr::mspResFont);
    mTextBoxYes->getFontSize(mFontSizeYes);

    mTextBoxNo = (J2DTextBox *)mScreenProgressive->search('T_no');
    mTextBoxNo->setFont(FontMgr::mspResFont);
    mTextBoxNo->getFontSize(mFontSizeNo);

    mTextBoxNum = (J2DTextBox *)mScreenProgressive->search('Num1');
    mTextBoxNum->setFont(FontMgr::mspResFont);

    {
        J2DPicture *pane = (J2DPicture *)mScreenProgressive->search('lyes');
#line 66
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPictureYes = pane;
    }
    mPictureYes->setAnimation(mTransform);

    {
        J2DPicture *pane = (J2DPicture *)mScreenProgressive->search('lno');
#line 72
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPictureNo = pane;
    }
    mPictureNo->setAnimation(mTransform);

    {
        J2DPicture *pane = (J2DPicture *)mScreenLogo->search('m_nin1');
#line 78
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPictureNintendo = pane;
    }

    {
        J2DPicture *pane = (J2DPicture *)mScreenLogo->search('Dlby0');
#line 83
        JUT_ASSERT(pane->getTypeID() == J2DPane_Picture);
        mPictureDolby = pane;
    }

    void *resource = JKRGetNameResource("Progressive.bpk", mArchive);
    for (int i = 0; i < 4; ++i)
    {
        mColors[i] = (J2DAnmColor *)J2DAnmLoaderDataBase::load(resource);
        mColors[i]->searchUpdateMaterialID(mScreenProgressive);
    }

    mTextBoxYes->setAnimation(mColors[0]);
    mTextBoxNo->setAnimation(mColors[1]);
    mPictureYes->setAnimation(mColors[2]);
    mPictureNo->setAnimation(mColors[3]);

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
    mTextBoxYes->setString(buffer);
    mTextBoxNo->setString(buffer);
    buffer[1] = '\0';
    mTextBoxNum->setString(buffer);
}

void PrintProgressive::init(PrintProgressive::MessageID messageID)
{
    mMsgId = messageID;

    mState = 2;
    mColorsFrame = 0;

    if (isChoice())
    {
        mOption = 0;
    }
    else
    {
        mOption = 2;
    }

    if (isChoice())
    {
        mPane->show();
        mTextBoxNum->show();
    }
    else
    {
        mPane->hide();
        mTextBoxNum->hide();
    }

    if (isMessage())
    {
        mTextBox->show();
    }
    else
    {
        mTextBox->hide();
    }

    if (isMessage())
    {
        int bmgMessageID = ReadPrintMessage::getBmgMessageID(mMsgId);
        char *message = ReadPrintMessage::getMessage(mProgressiveScanRes, bmgMessageID);

        mTextBox->setFontSize(
            mFontSize.x * ReadPrintMessage::getRate(mProgressiveScanRes, bmgMessageID) / 100.0f,
            mFontSize.y * ReadPrintMessage::getRate(mProgressiveScanRes, bmgMessageID) / 100.0f);

        mTextBox->setCharSpace(ReadPrintMessage::getCharSpace(mProgressiveScanRes, bmgMessageID));

        J2DTextBox::TFontSize fontSize;
        mTextBox->getFontSize(fontSize);
        char buf[512];
        ReadPrintMessage::tagCnv(message, mColorRes, fontSize, sizeof(buf), buf);
        strncpy(mTextBox->getStringPtr(), buf, sizeof(buf));
    }

    {
        char *message = ReadPrintMessage::getMessage(mYesNoRes, 1);

        mTextBoxYes->setFontSize(mFontSizeYes.x * ReadPrintMessage::getRate(mYesNoRes, 1) / 100.0f,
                                 mFontSizeYes.y * ReadPrintMessage::getRate(mYesNoRes, 1) / 100.0f);

        mTextBoxYes->setCharSpace(ReadPrintMessage::getCharSpace(mYesNoRes, 1));

        J2DTextBox::TFontSize fontSize;
        mTextBoxYes->getFontSize(fontSize);
        char buf[16];
        ReadPrintMessage::tagCnv(message, mColorRes, fontSize, sizeof(buf), buf);
        strncpy(mTextBoxYes->getStringPtr(), buf, sizeof(buf));
    }

    {
        char *message = ReadPrintMessage::getMessage(mYesNoRes, 0);

        mTextBoxNo->setFontSize(mFontSizeNo.x * ReadPrintMessage::getRate(mYesNoRes, 0) / 100.0f,
                                mFontSizeNo.y * ReadPrintMessage::getRate(mYesNoRes, 0) / 100.0f);

        mTextBoxNo->setCharSpace(ReadPrintMessage::getCharSpace(mYesNoRes, 0));

        J2DTextBox::TFontSize fontSize;
        mTextBoxNo->getFontSize(fontSize);
        char buf[16];
        ReadPrintMessage::tagCnv(message, mColorRes, fontSize, sizeof(buf), buf);
        strncpy(mTextBoxNo->getStringPtr(), buf, sizeof(buf));
    }

    switch (mMsgId)
    {
    case mcShow_Logo:
        mPictureNintendo->show();
        mPictureDolby->hide();
        break;
    case mcShow_Set:
        mPictureNintendo->hide();
        mPictureDolby->show();
        break;
    default:
        mPictureNintendo->hide();
        mPictureDolby->hide();
        break;
    }

    mTransformFrame = 0;
    mSTRKeyFrame = 0;
    mFrame = 539;
}

PrintProgressive::~PrintProgressive()
{
    // Unknown implementation.
}

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

void PrintProgressive::calc_bck()
{
    // Unknown implementation.
}

void PrintProgressive::calc_texChange()
{
    // Unknown implementation.
}

void PrintProgressive::calc(int param)
{
    if (param >= 0)
    {
        mFrame = param * 60;
    }

    switch (mMsgId)
    {
    case mcShow_Logo:
    case mcShow_Set:
        return;
    default:
        break;
    }

    if (isChoice())
    {
        if (gpaGamePad[0]->testTrigger(JUTGamePad::A))
        {
            if (mState == 2)
            {
                switch (mOption)
                {
                case 0:
                    mState = 0;
                    GameAudio::Main::getAudio()->startSystemSe(0x20003);
                    break;
                case 1:
                    mState = 1;
                    GameAudio::Main::getAudio()->startSystemSe(0x2000c);
                    break;
                default:
#line 372
                    JUT_ASSERT(0);
                }
            }
        }
        else if (gpaGamePad[0]->testRepeat(JUTGamePad::MAINSTICK_LEFT))
        {
            if (mState == 2)
            {
                switch (mOption)
                {
                case 1:
                    GameAudio::Main::getAudio()->startSystemSe(0x20000);
                    mOption = 0;
                    mColorsFrame = 0;
                    mTransformFrame = 0;
                    break;
                }
            }
        }
        else if (gpaGamePad[0]->testRepeat(JUTGamePad::MAINSTICK_RIGHT))
        {
            if (mState == 2)
            {
                switch (mOption)
                {
                case 0:
                    GameAudio::Main::getAudio()->startSystemSe(0x20000);
                    mOption = 1;
                    mColorsFrame = 0;
                    mTransformFrame = 0;
                    break;
                }
            }
        }
    }
    else if (gpaGamePad[0]->testTrigger(JUTGamePad::A))
    {
        if (mState == 2)
        {
            mState = 0;
        }
    }

    mSTRKeyFrame += 1;
    if (mSTRKeyFrame < 0)
    {
        mSTRKeyFrame = 0;
    }
    if (mSTRKeyFrame > 359)
    {
        mSTRKeyFrame = 0;
    }
    mSTRKey->setFrame(mSTRKeyFrame);

    if (isChoice())
    {
        switch (mOption)
        {
        case 0:
            mColors[0]->setFrame(1.0f);
            mColors[1]->setFrame(0.0f);
            if (mColorsFrame++ + 1 >= 359)
            {
                mColorsFrame = 0;
            }
            mColors[2]->setFrame(mColorsFrame);
            mColors[3]->setFrame(0.0f);
            break;
        case 1:
            mColors[0]->setFrame(0.0f);
            mColors[1]->setFrame(1.0f);
            if (mColorsFrame++ + 1 >= 359)
            {
                mColorsFrame = 0;
            }
            mColors[2]->setFrame(0.0f);
            mColors[3]->setFrame(mColorsFrame);
            break;
        default:
#line 465
            JUT_ASSERT(0);
        }
    }

    mScreenProgressive->animation();

    if (isChoice())
    {
        mTransformFrame += 1;
        if (mTransformFrame < 0)
        {
            mTransformFrame = 0;
        }
        if (mTransformFrame > 8)
        {
            mTransformFrame = 8;
        }

        switch (mOption)
        {
        case 0:
            mTransform->setFrame(mTransformFrame);
            mPictureYes->animationTransform();
            mTransform->setFrame(0.0f);
            mPictureNo->animationTransform();
            break;
        case 1:
            mTransform->setFrame(0.0f);
            mPictureYes->animationTransform();
            mTransform->setFrame(mTransformFrame);
            mPictureNo->animationTransform();
            break;
        }
    }

    if (isChoice())
    {
        if (mFrame > 0)
        {
            mFrame -= 1;
        }
        else
        {
            mFrame = 0;
        }

        char buff[2];
        buff[0] = getCounter() + '0';
        buff[1] = '\0';
        strcpy(mTextBoxNum->getStringPtr(), buff);
    }

    if (mFrame <= 0 && mState == 2)
    {
        GameAudio::Main::getAudio()->startSystemSe(0x20003);

        switch (mOption)
        {
        case 0:
            mState = 0;
            break;
        case 1:
            mState = 1;
            break;
        default:
#line 328
            JUT_ASSERT(0);
        }
    }
}

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
    s32 counter = -1;

    switch (mMsgId)
    {
    case mcShow_Dialog:
        if (mFrame != 0)
        {
            counter = mFrame / 60 + 1;
        }
        else
        {
            counter = 0;
        }
        break;
    default:
        break;
    }

    return counter;
}

void PrintProgressive::setCounter(int counter)
{
    // Unknown implementation.
}
