//=============================================================================================================
/**
* @file     realtimemultisamplearraydelegate.h
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     May, 2014
*
* @section  LICENSE
*
* Copyright (C) 2014, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Declaration of the RealTimeMultiSampleArrayDelegate Class.
*
*/

#ifndef REALTIMEMULTISAMPLEARRAYDELEGATE_H
#define REALTIMEMULTISAMPLEARRAYDELEGATE_H

//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QAbstractItemDelegate>
#include <QTableView>
#include <QMap>


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE XDISPLIB
//=============================================================================================================

namespace XDISPLIB
{


//=============================================================================================================
/**
* DECLARE CLASS RealTimeMultiSampleArrayDelegate
*
* @brief The RealTimeMultiSampleArrayDelegate class represents a RTMSA delegate which creates the plot paths
*/
class RealTimeMultiSampleArrayDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    //=========================================================================================================
    /**
    * Creates a new abstract item delegate with the given parent.
    *
    * @param[in] parent     Parent of the delegate
    */
    RealTimeMultiSampleArrayDelegate(QObject *parent = 0);

    //=========================================================================================================
    /**
    * Use the painter and style option to render the item specified by the item index.
    *
    * (sizeHint() must be implemented also)
    *
    * @param[in] painter    Low-level painting on widgets and other paint devices
    * @param[in] option     Describes the parameters used to draw an item in a view widget
    * @param[in] index      Used to locate data in a data model.
    */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    //=========================================================================================================
    /**
    * Item size
    *
    * @param[in] option     Describes the parameters used to draw an item in a view widget
    * @param[in] index      Used to locate data in a data model.
    */
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    //=========================================================================================================
    /**
    * createPlotPath creates the QPointer path for the data plot.
    *
    * @param[in] index QModelIndex for accessing associated data and model object.
    * @param[in,out] path The QPointerPath to create for the data plot.
    */
    void createPlotPath(const QModelIndex &index, const QStyleOptionViewItem &option, QPainterPath& path, QPainterPath& lastPath, QVector<float>& data, QVector<float>& lastData) const;

    //=========================================================================================================
    /**
    * createGridPath Creates the QPointer path for the grid plot.
    *
    * @param[in,out] path The row vector of the data matrix <1 x nsamples>.
    * @param[in] data The row vector of the data matrix <1 x nsamples>.
    */
    void createGridPath(const QModelIndex &index, const QStyleOptionViewItem &option, QPainterPath& path, QList<  QVector<float> >& data) const;

    //Settings
//    QSettings m_qSettings;

    // Scaling
    float m_fMaxValue;     /**< Maximum value of the data to plot  */
    float m_fScaleY;       /**< Maximum amplitude of plot (max is m_dPlotHeight/2) */
};

} // NAMESPACE

#endif // REALTIMEMULTISAMPLEARRAYDELEGATE_H
