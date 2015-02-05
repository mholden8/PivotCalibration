/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>

// SlicerQt includes
#include "qSlicerPivotCalibrationModuleWidget.h"
#include "ui_qSlicerPivotCalibrationModule.h"

#include "vtkSlicerPivotCalibrationLogic.h"

#include <vtkNew.h>
#include <vtkCommand.h>

#include <vtkMRMLLinearTransformNode.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerPivotCalibrationModuleWidgetPrivate: public Ui_qSlicerPivotCalibrationModule
{
  Q_DECLARE_PUBLIC( qSlicerPivotCalibrationModuleWidget );
protected:
  qSlicerPivotCalibrationModuleWidget* const q_ptr;
public:
  qSlicerPivotCalibrationModuleWidgetPrivate( qSlicerPivotCalibrationModuleWidget& object );
  vtkSlicerPivotCalibrationLogic* logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerPivotCalibrationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerPivotCalibrationModuleWidgetPrivate::qSlicerPivotCalibrationModuleWidgetPrivate( qSlicerPivotCalibrationModuleWidget& object )
 : q_ptr( &object )
{
}


vtkSlicerPivotCalibrationLogic* qSlicerPivotCalibrationModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerPivotCalibrationModuleWidget );
  return vtkSlicerPivotCalibrationLogic::SafeDownCast( q->logic() );
}


//-----------------------------------------------------------------------------
// qSlicerPivotCalibrationModuleWidget methods
//-----------------------------------------------------------------------------
qSlicerPivotCalibrationModuleWidget::qSlicerPivotCalibrationModuleWidget(QWidget* _parent) : Superclass( _parent ) , d_ptr( new qSlicerPivotCalibrationModuleWidgetPrivate(*this))
{
  this->pivotDelayTimer = new QTimer();
  pivotDelayTimer->setSingleShot(false);
  pivotDelayTimer->setInterval(1000);
  this->pivotDelayCount = 5;
  
  this->pivotSamplingTimer = new QTimer();
  pivotSamplingTimer->setSingleShot(false);
  pivotSamplingTimer->setInterval(1000);
  this->pivotSamplingCount = 5;
  
  this->spinDelayTimer = new QTimer();
  spinDelayTimer->setSingleShot(false);
  spinDelayTimer->setInterval(1000);
  this->spinDelayCount = 5;
  
  this->spinSamplingTimer = new QTimer();
  spinSamplingTimer->setSingleShot(false);
  spinSamplingTimer->setInterval(1000);
  this->spinSamplingCount = 5;

  this->timerSetting = 5;
}

//-----------------------------------------------------------------------------
qSlicerPivotCalibrationModuleWidget::~qSlicerPivotCalibrationModuleWidget()
{
  delete this->pivotDelayTimer;
  delete this->pivotSamplingTimer;
  delete this->spinDelayTimer;
  delete this->spinSamplingTimer;
}

//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::enter()
{
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::initializeObserver(vtkMRMLNode* node)
{
  Q_D(qSlicerPivotCalibrationModuleWidget);
  
  vtkMRMLLinearTransformNode* transformNode = vtkMRMLLinearTransformNode::SafeDownCast( node );
  d->logic()->InitializeObserver( transformNode );
}


//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::disableSpinCalibration()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);
  
  d->startSpinButton->setEnabled( false );
}


//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::setup()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);
  d->setupUi(this);
  

  this->Superclass::setup();
  
  connect(pivotDelayTimer, SIGNAL( timeout() ), this, SLOT( onPivotDelayTimeout() ));
  connect(pivotSamplingTimer, SIGNAL( timeout() ), this, SLOT( onPivotSamplingTimeout() ));
  connect(spinDelayTimer, SIGNAL( timeout() ), this, SLOT( onSpinDelayTimeout() ));
  connect(spinSamplingTimer, SIGNAL( timeout() ), this, SLOT( onSpinSamplingTimeout() )); 
  
  connect( d->InputComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(initializeObserver(vtkMRMLNode*)) );
  connect( d->InputComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(disableSpinCalibration()) );
  connect( d->OutputComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(disableSpinCalibration()) );
  
  connect( d->startPivotButton, SIGNAL( clicked() ), this, SLOT( onStartPivotPart() ) );
  connect( d->startSpinButton, SIGNAL( clicked() ), this, SLOT( onStartSpinPart() ) );
  
  connect( d->timerEdit, SIGNAL( valueChanged(double) ), this, SLOT( setTimer(double) ) );
}

//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::onStartPivotPart()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);

  this->pivotDelayCount = this->timerSetting;
  this->pivotSamplingCount = this->timerSetting;

  std::stringstream ss;
  ss << this->pivotDelayCount << " seconds until start";
  d->CountdownLabel->setText(ss.str().c_str());  
  
  pivotDelayTimer->start();
}

//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::onStartSpinPart()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);

  this->spinDelayCount = this->timerSetting;
  this->spinSamplingCount = this->timerSetting;

  std::stringstream ss;
  ss << this->spinDelayCount << " seconds until start";
  d->CountdownLabel->setText(ss.str().c_str());  
  
  spinDelayTimer->start();
}


//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::onPivotDelayTimeout()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);

  std::stringstream ss1;
  
  this->pivotDelayCount -= 1;
  ss1 << this->pivotDelayCount << " seconds until start";
  d->CountdownLabel->setText(ss1.str().c_str());

  if (this->pivotDelayCount <= 0)
  {
    std::stringstream ss2;
    this->pivotSamplingCount = this->timerSetting;
    ss2 << "Sampling time left: " << this->pivotSamplingCount;
    d->CountdownLabel->setText(ss2.str().c_str());
    
    this->pivotDelayTimer->stop();
    d->logic()->SetRecordingState(true);
    this->pivotSamplingTimer->start();
  }
}

//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::onPivotSamplingTimeout()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);
  
  std::stringstream ss;

  //std::cout<<this->samplingCount<<std::endl;
  
  this->pivotSamplingCount -= 1;
  ss << "Sampling time left: " << this->pivotSamplingCount;
  d->CountdownLabel->setText(ss.str().c_str());  
  
  if (this->pivotSamplingCount <= 0)
  {
    d->CountdownLabel->setText("Sampling complete");
    
    this->pivotSamplingTimer->stop();
    this->onPivotStop();
  }  
}


//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::onSpinDelayTimeout()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);

  std::stringstream ss1;
  
  this->spinDelayCount -= 1;
  ss1 << this->spinDelayCount << " seconds until start";
  d->CountdownLabel->setText(ss1.str().c_str());

  if (this->spinDelayCount <= 0)
  {
    std::stringstream ss2;
    this->spinSamplingCount = this->timerSetting;
    ss2 << "Sampling time left: " << this->spinSamplingCount;
    d->CountdownLabel->setText(ss2.str().c_str());
    
    this->spinDelayTimer->stop();
    d->logic()->SetRecordingState(true);
    this->spinSamplingTimer->start();
  }
}

//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::onSpinSamplingTimeout()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);
  
  std::stringstream ss;

  //std::cout<<this->samplingCount<<std::endl;
  
  this->spinSamplingCount -= 1;
  ss << "Sampling time left: " << this->spinSamplingCount;
  d->CountdownLabel->setText(ss.str().c_str());  
  
  if (this->spinSamplingCount <= 0)
  {
    d->CountdownLabel->setText("Sampling complete");
    
    this->spinSamplingTimer->stop();
    this->onSpinStop();
  }  
}


//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::onPivotStop()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);
  
  d->logic()->SetRecordingState(false);
  d->logic()->ComputePivotCalibration();
  
  vtkMRMLLinearTransformNode* outputTransform = vtkMRMLLinearTransformNode::SafeDownCast(d->OutputComboBox->currentNode());
#ifdef TRANSFORM_NODE_MATRIX_COPY_REQUIRED
  vtkSmartPointer<vtkMatrix4x4> outputMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  outputTransform->GetMatrixTransformToParent(outputMatrix);
#else
  vtkMatrix4x4* outputMatrix = outputTransform->GetMatrixTransformToParent();
#endif

  d->logic()->GetToolTipToToolMatrix( outputMatrix );

#ifdef TRANSFORM_NODE_MATRIX_COPY_REQUIRED
  outputTransform->SetMatrixTransformToParent(outputMatrix);
#endif
  
  std::stringstream ss;
  ss << d->logic()->GetPivotRMSE();
  d->rmseLabel->setText(ss.str().c_str());
  
  d->logic()->ClearToolToReferenceMatrices();

  d->startSpinButton->setEnabled( true );
}


//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::onSpinStop()
{
  Q_D(qSlicerPivotCalibrationModuleWidget);
  
  d->logic()->SetRecordingState(false);
  d->logic()->ComputeSpinCalibration();

  if ( d->snapCheckBox->checkState() == Qt::Checked )
  {
    d->logic()->SnapRotationRightAngle();
  }

  vtkMRMLLinearTransformNode* outputTransform = vtkMRMLLinearTransformNode::SafeDownCast(d->OutputComboBox->currentNode());
#ifdef TRANSFORM_NODE_MATRIX_COPY_REQUIRED
  vtkSmartPointer<vtkMatrix4x4> outputMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  outputTransform->GetMatrixTransformToParent(outputMatrix);
#else
  vtkMatrix4x4* outputMatrix = outputTransform->GetMatrixTransformToParent();
#endif

  d->logic()->GetToolTipToToolMatrix( outputMatrix );

#ifdef TRANSFORM_NODE_MATRIX_COPY_REQUIRED
  outputTransform->SetMatrixTransformToParent(outputMatrix);
#endif
   
  // Set the rmse label for the circle fitting rms error
  std::stringstream ss;
  ss << d->logic()->GetSpinRMSE();
  d->rmseLabel->setText(ss.str().c_str());
  
  d->logic()->ClearToolToReferenceMatrices();
}


//-----------------------------------------------------------------------------
void qSlicerPivotCalibrationModuleWidget::setTimer(double time)
{
  this->timerSetting = (int)time;
}

