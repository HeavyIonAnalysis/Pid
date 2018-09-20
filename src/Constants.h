/** @file   Constants.h
    @author Viktor Klochkov (klochkov44@gmail.com)
    @author Ilya Selyuzhenkov (ilya.selyuzhenkov@gmail.com)
    @brief  Some constants and enumerators
*/


#ifndef PidConstants_H
#define PidConstants_H 1

namespace PidParticles{
    enum eParticle{
        kProton = 0,
        kPion,
        kKaon,
        kBg,
        nParticles
    };
    
    const float masses[nParticles] = { 0.938, 0.140, 0.5, 0.0 };
    
}

namespace PidFunction{
    enum eNames{
        kA = 0,
        kMean,
        kSigma,
        nParams
    };    
}


#endif
