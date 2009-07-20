/* testqt.cc: Example app for clutter-qt
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

#include <QApplication>
#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <glib.h>

#include <clutter/clutter.h>
#include <clutter-qt/clutter-qt.h>
#include "testqt.h"

struct SpinControl
{
  const char *name;
  const char *value_slot, *animate_slot;
  int default_value, max;
};

static SpinControl
spin_controls[] = { { "Rotate x-axis",
                      SLOT (onSpinX(int)),
                      SLOT (onAnimateX(int)),
                      0, 360 },
                    { "Rotate y-axis",
                      SLOT (onSpinY(int)),
                      SLOT (onAnimateY(int)),
                      0, 360 },
                    { "Rotate z-axis",
                      SLOT (onSpinZ(int)),
                      SLOT (onAnimateZ(int)),
                      0, 360 },
                    { "Adjust opacity",
                      SLOT (onAdjustOpacity(int)),
                      SLOT (onAnimateOpacity(int)),
                      255, 255 } };

#define STAGE_WIDTH  640
#define STAGE_HEIGHT 480

MainWin::MainWin (QWidget *parent)
  : QWidget (parent)
{
  cqt.setMinimumSize (QSize (STAGE_WIDTH, STAGE_HEIGHT));
  cqt.setFocusPolicy(Qt::StrongFocus);
  const QColor &bg_color = cqt.palette ().color (QPalette::Window);
  ClutterColor clutter_bg_color = { bg_color.red (),
                                    bg_color.green (),
                                    bg_color.blue (),
                                    255 };
  clutter_stage_set_color (CLUTTER_STAGE (cqt.stage ()), &clutter_bg_color);

  layout.addWidget (&edit, 0, 0, 1, 4);
  layout.addWidget (&cqt, 1, 0, G_N_ELEMENTS (spin_controls) + 1, 1);

  for (int i = 0; i < (int) G_N_ELEMENTS (spin_controls); i++)
    {
      QLabel *label = new QLabel (spin_controls[i].name);
      layout.addWidget (label, i + 1, 1);

      QSpinBox *spin_box = new QSpinBox;
      spin_box->setRange (0, spin_controls[i].max);
      spin_box->setValue (spin_controls[i].default_value);
      layout.addWidget (spin_box, i + 1, 2);
      QObject::connect (spin_box, SIGNAL (valueChanged(int)),
                        this, spin_controls[i].value_slot);

      QCheckBox *check_box = new QCheckBox("Animate");
      layout.addWidget (check_box, i + 1, 3);
      QObject::connect (check_box, SIGNAL (stateChanged(int)),
                        this, spin_controls[i].animate_slot);
    }

  hand = clutter_texture_new_from_file ("redhand.png", NULL);
  clutter_container_add (CLUTTER_CONTAINER (cqt.stage ()), hand, NULL);
  clutter_actor_set_position (hand,
                              STAGE_WIDTH / 2
                              - clutter_actor_get_width (hand) / 2,
                              STAGE_HEIGHT / 2
                              - clutter_actor_get_height (hand) / 2);

  entry = clutter_text_new_with_text ("Sans 10", "");
  clutter_text_set_editable (CLUTTER_TEXT (entry), TRUE);
  clutter_actor_set_size (entry, 500, 20);
  clutter_container_add (CLUTTER_CONTAINER (cqt.stage ()), entry, NULL);
  QObject::connect (&edit, SIGNAL (textChanged(const QString&)),
                    this, SLOT (onEditChanged(const QString&)));

  tl = clutter_timeline_new (8000);
  clutter_timeline_set_loop (tl, TRUE);
  clutter_timeline_start (tl);
  g_signal_connect (tl, "new-frame",
                    G_CALLBACK (MainWin::onNewFrame), this);

  animateX = false;
  animateY = false;
  animateZ = false;
  animateOpacity = false;

  setLayout (&layout);

  g_signal_connect (cqt.stage (), "event", G_CALLBACK (onInput), this);
}

MainWin::~MainWin ()
{
  g_object_unref (tl);
}

void
MainWin::onEditChanged (const QString &str)
{
  clutter_text_set_text (CLUTTER_TEXT (entry), str.toUtf8 ().constData ());
}

void
MainWin::onSpinX (int angle)
{
  clutter_actor_set_rotation (hand, CLUTTER_X_AXIS,
                              angle,
                              0,
                              clutter_actor_get_height (hand) / 2,
                              0);
}

void
MainWin::onSpinY (int angle)
{
  clutter_actor_set_rotation (hand, CLUTTER_Y_AXIS,
                              angle,
                              clutter_actor_get_width (hand) / 2,
                              0,
                              0);
}

void
MainWin::onSpinZ (int angle)
{
  clutter_actor_set_rotation (hand, CLUTTER_Z_AXIS,
                              angle,
                              clutter_actor_get_width (hand) / 2,
                              clutter_actor_get_height (hand) / 2,
                              0);
}

void
MainWin::onAdjustOpacity (int opacity)
{
  clutter_actor_set_opacity (hand, opacity);
}

void
MainWin::onAnimateX (int state)
{
  animateX = state == Qt::Checked;
}

void
MainWin::onAnimateY (int state)
{
  animateY = state == Qt::Checked;
}

void
MainWin::onAnimateZ (int state)
{
  animateZ = state == Qt::Checked;
}

void
MainWin::onAnimateOpacity (int state)
{
  animateOpacity = state == Qt::Checked;
}

void
MainWin::onNewFrame (ClutterTimeline *tl, gint frame_num, MainWin *main_win)
{
  gdouble progress = clutter_timeline_get_progress (tl) * 2.0;

  if (progress >= 1.0)
    progress = 2.0 - progress;

  int angle = progress * 360;
  int opacity = progress * 255;

  if (main_win->animateX)
    main_win->onSpinX (angle);
  if (main_win->animateY)
    main_win->onSpinY (angle);
  if (main_win->animateZ)
    main_win->onSpinZ (angle);

  if (main_win->animateOpacity)
    main_win->onAdjustOpacity (opacity);
}

void
MainWin::fillKeybuf (char *keybuf, ClutterKeyEvent *event)
{
  char utf8[6];
  int len;

  /* printable character, if any (ß, ∑) */
  len = g_unichar_to_utf8 (event->unicode_value, utf8);
  utf8[len] = '\0';
  sprintf(keybuf, "'%s' ", utf8);

  /* key combination (<Mod1>s, <Shift><Mod1>S, <Ctrl><Mod1>Delete) */
  len = g_unichar_to_utf8 (clutter_keysym_to_unicode (event->keyval),
                           utf8);
  utf8[len] = '\0';

  if (event->modifier_state & CLUTTER_SHIFT_MASK)
    strcat (keybuf, "<Shift>");
  if (event->modifier_state & CLUTTER_LOCK_MASK)
    strcat (keybuf, "<Lock>");
  if (event->modifier_state & CLUTTER_CONTROL_MASK)
    strcat (keybuf, "<Control>");
  if (event->modifier_state & CLUTTER_MOD1_MASK)
    strcat (keybuf, "<Mod1>");
  if (event->modifier_state & CLUTTER_MOD2_MASK)
    strcat (keybuf, "<Mod2>");
  if (event->modifier_state & CLUTTER_MOD3_MASK)
    strcat (keybuf, "<Mod3>");
  if (event->modifier_state & CLUTTER_MOD4_MASK)
    strcat (keybuf, "<Mod4>");
  if (event->modifier_state & CLUTTER_MOD5_MASK)
    strcat (keybuf, "<Mod5>");
  strcat (keybuf, utf8);
}

void
MainWin::printMouseModifiers (ClutterModifierType state)
{
  if (state & (CLUTTER_BUTTON1_MASK | CLUTTER_BUTTON2_MASK
               | CLUTTER_BUTTON3_MASK))
    {
      printf (" Mouse(");
      if ((state & CLUTTER_BUTTON1_MASK))
        printf ("1");
      if ((state & CLUTTER_BUTTON2_MASK))
        printf ("2");
      if ((state & CLUTTER_BUTTON3_MASK))
        printf ("3");
      fputc (')', stdout);
    }
}

gboolean
MainWin::onInput (ClutterActor    *actor,
                  ClutterEvent    *event,
                  MainWin         *main_win)
{
  ClutterStage *stage = CLUTTER_STAGE (main_win->cqt.stage ());
  gchar keybuf[128];

  switch (event->type)
    {
    case CLUTTER_KEY_PRESS:
      fillKeybuf (keybuf, &event->key);
      printf ("KEY PRESS %s", keybuf);
      printMouseModifiers (event->key.modifier_state);
      break;
    case CLUTTER_KEY_RELEASE:
      fillKeybuf (keybuf, &event->key);
      printf ("KEY RELEASE %s", keybuf);
      printMouseModifiers (event->key.modifier_state);
      break;
    case CLUTTER_MOTION:
      g_print ("MOTION");
      printMouseModifiers (event->motion.modifier_state);
      break;
    case CLUTTER_ENTER:
      g_print ("ENTER");
      break;
    case CLUTTER_LEAVE:
      g_print ("LEAVE");
      break;
    case CLUTTER_BUTTON_PRESS:
      g_print ("BUTTON PRESS (click count:%i)",
               event->button.click_count);
      printMouseModifiers (event->button.modifier_state);
      break;
    case CLUTTER_BUTTON_RELEASE:
      g_print ("BUTTON RELEASE (click count:%i)",
               event->button.click_count);
      printMouseModifiers (event->button.modifier_state);

      if (clutter_event_get_source (event) == CLUTTER_ACTOR (stage))
        clutter_stage_set_key_focus (stage, NULL);
      else if (clutter_event_get_source (event) == actor
               && clutter_actor_get_parent (actor) == CLUTTER_ACTOR (stage))
        clutter_stage_set_key_focus (stage, actor);
      break;
    case CLUTTER_SCROLL:
      g_print ("BUTTON SCROLL %s (click count:%i)",
               event->scroll.direction == CLUTTER_SCROLL_UP ? "up" : "down",
               event->button.click_count);
      break;
    default:
      return FALSE;
    }

  if (clutter_event_get_source (event) == actor)
    g_print (" *source*");

  g_print ("\n");

  return FALSE;
}

int
main (int argc, char **argv)
{
  QApplication app (argc, argv);

  ClutterQt::init (&argc, &argv);

  MainWin win;
  win.setWindowTitle ("QT-Clutter Interaction demo");

  win.show ();

  return app.exec ();
}
