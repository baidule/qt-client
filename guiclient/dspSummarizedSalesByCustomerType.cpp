/*
 * This file is part of the xTuple ERP: PostBooks Edition, a free and
 * open source Enterprise Resource Planning software suite,
 * Copyright (c) 1999-2010 by OpenMFG LLC, d/b/a xTuple.
 * It is licensed to you under the Common Public Attribution License
 * version 1.0, the full text of which (including xTuple-specific Exhibits)
 * is available at www.xtuple.com/CPAL.  By using this software, you agree
 * to be bound by its terms.
 */

#include "dspSummarizedSalesByCustomerType.h"

#include <QVariant>
#include <QMessageBox>
//#include <QStatusBar>

#include <parameter.h>
#include <openreports.h>
#include <metasql.h>

#include "mqlutil.h"

/*
 *  Constructs a dspSummarizedSalesByCustomerType as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
dspSummarizedSalesByCustomerType::dspSummarizedSalesByCustomerType(QWidget* parent, const char* name, Qt::WFlags fl)
    : XWidget(parent, name, fl)
{
  setupUi(this);

//  (void)statusBar();

  // signals and slots connections
  connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
  connect(_close, SIGNAL(clicked()), this, SLOT(close()));
  connect(_query, SIGNAL(clicked()), this, SLOT(sFillList()));

  _customerType->setType(ParameterGroup::CustomerType);

  _sohist->addColumn(tr("Customer Type"),    -1,              Qt::AlignLeft,   true,  "custtype_code"   );
  _sohist->addColumn(tr("Site"),             _whsColumn,      Qt::AlignCenter, true,  "warehous_code" );
  _sohist->addColumn(tr("Min. Price"),       _priceColumn,    Qt::AlignRight,  true,  "minprice"  );
  _sohist->addColumn(tr("Max. Price"),       _priceColumn,    Qt::AlignRight,  true,  "maxprice"  );
  _sohist->addColumn(tr("Avg. Price"),       _priceColumn,    Qt::AlignRight,  true,  "avgprice"  );
  _sohist->addColumn(tr("Wt. Avg. Price"),   _priceColumn,    Qt::AlignRight,  true,  "wtavgprice"  );
  _sohist->addColumn(tr("Total Units"),      _qtyColumn,      Qt::AlignRight,  true,  "totalunits"  );
  _sohist->addColumn(tr("Total Sales"),      _bigMoneyColumn, Qt::AlignRight,  true,  "totalsales"  );
  _sohist->setDragString("custtypeid=");
}

/*
 *  Destroys the object and frees any allocated resources
 */
dspSummarizedSalesByCustomerType::~dspSummarizedSalesByCustomerType()
{
  // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dspSummarizedSalesByCustomerType::languageChange()
{
  retranslateUi(this);
}

void dspSummarizedSalesByCustomerType::sPrint()
{
  if (!_dates->startDate().isValid())
  {
    QMessageBox::warning( this, tr("Enter Start Date"),
                          tr("Please enter a valid Start Date.") );
    _dates->setFocus();
    return;
  }

  if (!_dates->endDate().isValid())
  {
    QMessageBox::warning( this, tr("Enter End Date"),
                          tr("Please enter a valid End Date.") );
    _dates->setFocus();
    return;
  }

  ParameterList params;
  _customerType->appendValue(params);
  _warehouse->appendValue(params);
  _dates->appendValue(params);

  orReport report("SummarizedSalesByCustomerType", params);
  if (report.isValid())
    report.print();
  else
    report.reportError(this);
}

void dspSummarizedSalesByCustomerType::sFillList()
{
  _sohist->clear();

  if (!checkParameters())
    return;

  MetaSQLQuery mql = mqlLoad("summarizedSalesHistory", "detail");
  ParameterList params;
  setParams(params);
  q = mql.toQuery(params);
  _sohist->populate(q);
}

bool dspSummarizedSalesByCustomerType::checkParameters()
{
  if (!_dates->startDate().isValid())
  {
    if(isVisible()) {
      QMessageBox::warning( this, tr("Enter Start Date"),
                            tr("Please enter a valid Start Date.") );
      _dates->setFocus();
    }
    return FALSE;
  }

  if (!_dates->endDate().isValid())
  {
    if(isVisible()) {
      QMessageBox::warning( this, tr("Enter End Date"),
                            tr("Please enter a valid End Date.") );
      _dates->setFocus();
    }
    return FALSE;
  }

  if (_customerType->isPattern())
  {
    QString pattern = _customerType->pattern();
    if (pattern.length() == 0)
      return FALSE;
  }

  return TRUE;
}

void dspSummarizedSalesByCustomerType::setParams(ParameterList & params)
{
  params.append("startDate", _dates->startDate());
  params.append("endDate", _dates->endDate());

  if (_warehouse->isSelected())
    params.append("warehous_id", _warehouse->id());

  if (_customerType->isSelected())
    params.append("custtype_id", _customerType->id());
  else if (_customerType->isPattern())
  {
    params.append("custtype_pattern", _customerType->pattern());
  }

  params.append("byCustomerType");
  params.append("commonParamsSet1");
}
