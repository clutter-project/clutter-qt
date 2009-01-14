/* testqt.h: Example app for clutter-qt
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

#ifndef TEST_QT_H
#define TEST_QT_H

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>

#include <clutter/clutter.h>
#include <clutter-qt/clutter-qt.h>

class MainWin : public QWidget
{
  Q_OBJECT;

public:
  MainWin (QWidget *parent = NULL);
  ~MainWin ();

public slots:
  void onEditChanged (const QString &str);
  void onSpinX (int angle);
  void onSpinY (int angle);
  void onSpinZ (int angle);
  void onAdjustOpacity (int opacity);
  void onAnimateX (int state);
  void onAnimateY (int state);
  void onAnimateZ (int state);
  void onAnimateOpacity (int state);

private:
  ClutterActor *hand, *entry;
  ClutterTimeline *tl;

  ClutterQt cqt;
  QLineEdit edit;
  QGridLayout layout;
  
  bool animateX, animateY, animateZ, animateOpacity;

  static void onNewFrame (ClutterTimeline *tl, gint frame_num,
			  MainWin *main_win);
  static void fillKeybuf (char *keybuf, ClutterKeyEvent *event);
  static gboolean onInput (ClutterActor *actor, ClutterEvent *event,
			   MainWin *main_win);
  static void printMouseModifiers (ClutterModifierType state);
};


#endif /* TEST_QT_H */
