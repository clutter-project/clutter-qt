/* clutter-qt.h: Embeddable ClutterStage
 *
 * Copyright (C) 2008 OpenedHand
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not see <http://www.fsf.org/licensing>.
 *
 * Authors:
 *   Neil Roberts <neil@o-hand.com>
 */

#ifndef CLUTTER_QT_H
#define CLUTTER_QT_H

#include <QWidget>
#include <QInputEvent>
#include <clutter/clutter.h>

class ClutterQtPrivate;

class ClutterQt : public QWidget
{
  Q_OBJECT;

public:
  ClutterQt (QWidget *parent = 0);
  ~ClutterQt ();

  ClutterActor *stage () const;

  static ClutterInitError init (int *argc, char ***argv);

protected:
  virtual void resizeEvent (QResizeEvent *event);
  virtual void changeEvent (QEvent *event);
  virtual void paintEvent (QPaintEvent *event);
  virtual void showEvent (QShowEvent *event);
  virtual void hideEvent (QHideEvent *event);
  virtual void mouseMoveEvent (QMouseEvent *event);
  virtual void mousePressEvent (QMouseEvent *event);
  virtual void mouseReleaseEvent (QMouseEvent *event);
  virtual void keyPressEvent (QKeyEvent *event);
  virtual void keyReleaseEvent (QKeyEvent *event);

private:
  ClutterQtPrivate *priv;

  enum { MIN_WHEEL_DELTA = 120 };

  void resetWindow (QWidget *parent);
  void mousePressOrReleaseEvent (QMouseEvent *event);
  void keyPressOrReleaseEvent (QKeyEvent *event);
  void wheelEvent (QWheelEvent *event);
  static ClutterModifierType getModifierState (QInputEvent *event);
};

#endif /* CLUTTER_QT_H */
