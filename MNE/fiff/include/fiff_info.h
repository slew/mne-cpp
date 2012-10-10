//=============================================================================================================
/**
* @file     fiff_info.h
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti H�m�l�inen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     July, 2012
*
* @section  LICENSE
*
* Copyright (C) 2012, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Contains the FiffInfo class declaration.
*
*/

#ifndef FIFF_INFO_H
#define FIFF_INFO_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "../fiff_global.h"
#include "fiff_types.h"
#include "fiff_id.h"
#include "fiff_ch_info.h"
#include "fiff_dig_point.h"
#include "fiff_ctf_comp.h"
#include "fiff_coord_trans.h"
#include "fiff_proj.h"


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QList>
#include <QStringList>


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE FIFFLIB
//=============================================================================================================

namespace FIFFLIB
{


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================


//=============================================================================================================
/**
* Provides fiff measurement file information
*
* @brief FIFF measurement file information
*/
class FIFFSHARED_EXPORT FiffInfo {

public:
    //=========================================================================================================
    /**
    * ctor
    */
    FiffInfo();

    //=========================================================================================================
    /**
    * Destroys the FiffInfo.
    */
    ~FiffInfo();


    //=========================================================================================================
    /**
    * mne_make_projector
    *
    * ### MNE toolbox root function ### Implementation of the mne_make_projector function
    *
    *
    * Make an SSP operator
    *
    * @param[in] projs      A set of projection vectors
    * @param[in] ch_names   A cell array of channel names
    * @param[out] proj      The projection operator to apply to the data
    * @param[in] bads       Bad channels to exclude
    * @param[out] U         The orthogonal basis of the projection vectors (optional)
    *
    * @return nproj - How many items in the projector
    */
    static fiff_int_t make_projector(QList<FiffProj*>& projs, QStringList& ch_names, MatrixXf& proj, QStringList& bads = defaultQStringList, MatrixXf& U = defaultUMatrix);


    //=========================================================================================================
    /**
    * Make a SSP operator using the meas info
    *
    * @param[in] info       Fiff measurement info
    * @param[out] proj      The projection operator to apply to the data
    *
    * @return nproj - How many items in the projector
    */
    inline qint32 make_projector_info(MatrixXf& proj)
    {
        return make_projector(this->projs,this->ch_names, proj, this->bads);
    }


    //=========================================================================================================
    /**
    * mne_make_projector_info
    *
    * ### MNE toolbox root function ###  Implementation of the mne_make_projector_info function
    *
    * The member function should be prefered: make_projector_info(MatrixXf& proj)
    *
    * Make a SSP operator using the meas info
    *
    * @param[in] info       Fiff measurement info
    * @param[out] proj      The projection operator to apply to the data
    *
    * @return nproj - How many items in the projector
    */
    inline static qint32 make_projector_info(FiffInfo* info, MatrixXf& proj)
    {
        return info->make_projector_info(proj);
    }



public:
    FiffId      file_id;
    FiffId      meas_id;
    fiff_int_t  meas_date[2];
    fiff_int_t  nchan;
    float sfreq;
    float highpass;
    float lowpass;
    QList<FiffChInfo> chs;
    QStringList ch_names;
    FiffCoordTrans dev_head_t;
    FiffCoordTrans ctf_head_t;
    FiffCoordTrans dev_ctf_t;
    QList<FiffDigPoint> dig;
    FiffCoordTrans dig_trans;
    QStringList bads;
    QList<FiffProj*> projs;
    QList<FiffCtfComp*> comps;
    QString acq_pars;
    QString acq_stim;
    QString filename;
};

} // NAMESPACE

#endif // FIFF_INFO_H