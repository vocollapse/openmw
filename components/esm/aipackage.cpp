#include "aipackage.hpp"

#include "esmreader.hpp"
#include "esmwriter.hpp"

namespace ESM
{
    void AIData::blank()
    {
        mHello = mU1 = mFight = mFlee = mAlarm = mU2 = mU3 = mU4 = 0;
        mServices = 0;
    }

    void AIPackageList::load(ESMReader &esm)
    {
        mList.clear();
        while (esm.hasMoreSubs()) {
            // initialize every iteration
            AIPackage pack;
                esm.getSubName();
            if (esm.retSubName() == 0x54444e43) { // CNDT
                mList.back().mCellName = esm.getHString();
            } else if (esm.retSubName() == AI_Wander) {
                pack.mType = AI_Wander;
                esm.getHExact(&pack.mWander, 14);
                mList.push_back(pack);
            } else if (esm.retSubName() == AI_Travel) {
                pack.mType = AI_Travel;
                esm.getHExact(&pack.mTravel, 16);
                mList.push_back(pack);
            } else if (esm.retSubName() == AI_Escort ||
                       esm.retSubName() == AI_Follow)
            {
                pack.mType =
                    (esm.retSubName() == AI_Escort) ? AI_Escort : AI_Follow;
                    esm.getHExact(&pack.mTarget, 48);
                mList.push_back(pack);
            } else if (esm.retSubName() == AI_Activate) {
                pack.mType = AI_Activate;
                esm.getHExact(&pack.mActivate, 33);
                mList.push_back(pack);
            } else { // not AI package related data, so leave
                 return;
            }
        }
    }

    void AIPackageList::save(ESMWriter &esm) const
    {
        typedef std::vector<AIPackage>::const_iterator PackageIter;
        for (PackageIter it = mList.begin(); it != mList.end(); ++it) {
            switch (it->mType) {
            case AI_Wander:
                esm.writeHNT("AI_W", it->mWander, sizeof(it->mWander));
                break;

            case AI_Travel:
                esm.writeHNT("AI_T", it->mTravel, sizeof(it->mTravel));
                break;

            case AI_Activate:
                esm.writeHNT("AI_A", it->mActivate, sizeof(it->mActivate));
                break;

            case AI_Escort:
            case AI_Follow: {
                const char *name = (it->mType == AI_Escort) ? "AI_E" : "AI_F";
                esm.writeHNT(name, it->mTarget, sizeof(it->mTarget));
                esm.writeHNOCString("CNDT", it->mCellName);
                break;
            }

            default:
                break;
            }
        }
    }
}
