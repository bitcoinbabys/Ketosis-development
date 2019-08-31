// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017 The Signatum Project www.signatum.io
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (      0,     uint256("0x000009f28c827b0887a28e52427e958949577071cac33e1597a1238a27ffdbb8") )
        (      50,     uint256("0xc22b8dcd92d018701df9cb289ed9bc4d2eb38adb8bff37d159a29c78c7f25ded") )
        (      250,     uint256("0x1c686094d06e23fadba2d2ba3a476979676c8bd45a319a0c25579b07232bcac7") )
        (      1000,     uint256("0x94939f010d0d362b0e7906d8d089e156fe33530784140d9e45baf90c22b7bd71") )
        (      5000,     uint256("0x40a38df9f173a2c2e474d766eac196836ea34aad31e1df248d05d221d70852b8") )
        (      20000,     uint256("0x5003e3f82fb40bdb3f8e087861fafad27a43990a321b5af4e39b2ba4ff11669e") )
        (      50000,     uint256("0x17bb356555d1d4f685fb9775cd8227864e7c4848ce492e84543cd55d45fe1803") )
        (      150000,     uint256("0x39e638fd7d6b758075a5a173f424e9b57eafcf7ba80f65c8155e342886edfc31") )
        (      350000,     uint256("0xcdd3b2727df567cfe7f15d1a6098e91a78d8b40fef2550b478927e52b21e7c97") )

        ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
