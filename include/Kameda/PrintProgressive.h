#ifndef PRINTPROGRESSIVE_H
#define PRINTPROGRESSIVE_H

#include "JSystem/J2D/J2DGrafContext.h"
#include "JSystem/J2D/J2DPane.h"
#include "JSystem/J2D/J2DPicture.h"
#include "JSystem/J2D/J2DScreen.h"
#include "JSystem/JKernel/JKRArchive.h"
#include "JSystem/JKernel/JKRHeap.h"
#include "types.h"

class PrintProgressive
{
public:
    enum MessageID  // according to debug test screen
    {
        mcShow_Dialog,
        mcSet_Option1,
        mcSet_Option2,
        mcShow_Logo,
        mcShow_Set
    };

    PrintProgressive(JKRHeap *heap);  // 0x8016aac4
    void init(MessageID);             // 0x8016b138
    void draw();                      // 0x8016b65c
    void calc(int);                   // 0x8016b6dc
    bool isMessage();                 // 0x8016bc28
    bool isChoice();                  // 0x8016bcbc
    s32 getCounter();                 // 0x8016bd48
    // Inline/Unused
    ~PrintProgressive();
    void calc_bck();
    void calc_texChange();
    void setCounter(int);

    int getState() { return mState; }
    int getOption() { return mOption; }

private:
    void *mColorRes;                   // 0x0
    void *mProgressiveScanRes;         // 0x4
    void *mYesNoRes;                   // 0x8
    J2DOrthoGraph *mOrthoGraph;        // 0xc
    JKRArchive *mArchive;              // 0x10
    J2DScreen *mScreenLogo;            // 0x14
    J2DScreen *mScreenProgressive;     // 0x18
    J2DPicture *mPane;                 // 0x1c
    J2DTextBox *mTextBox;              // 0x20
    J2DTextBox *mTextBox2;             // 0x24
    J2DTextBox *mTextBox3;             // 0x28
    J2DTextBox *mTextBox4;             // 0x2c
    J2DPicture *mPane2;                // 0x30
    J2DPicture *mPane3;                // 0x34
    J2DPicture *mPane4;                // 0x38
    J2DPicture *mPane5;                // 0x3c
    J2DAnmTransform *mTransform;       // 0x40
    J2DAnmColor *mColors[4];           // 0x44
    J2DAnmTextureSRTKey *mSTRKey;      // 0x54
    u8 _58[4];                         // 0x58
    u8 _5c[4];                         // 0x5c
    s32 _60;                           // 0x60
    MessageID mMsgId;                  // 0x64
    int mState;                        // 0x68
    int mOption;                       // 0x6c
    u32 _70;                           // 0x70
    J2DTextBox::TFontSize mFontSize;   // 0x74
    J2DTextBox::TFontSize mFontSize2;  // 0x7c
    J2DTextBox::TFontSize mFontSize3;  // 0x84
    J2DTextBox::TFontSize mFontSize4;  // 0x8c
};  // Size: 0x94

#endif  // PRINTPROGRESSIVE_H
