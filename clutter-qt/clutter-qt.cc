/* clutter-qt.cc: Embeddable ClutterStage
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

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QInputEvent>
#include <QTime>
#include <QX11Info>

#include <clutter/clutter-stage.h>
#include <clutter/x11/clutter-x11.h>

/* These two are defined in the X11 headers included by clutter-x11.h
   but they break the namespaced QT event numbers */
#undef KeyPress
#undef KeyRelease

#include "clutter-qt.h"

struct ClutterQtPrivate
{
  ClutterActor *stage;

  QTime time;

  int scrollPos;
};

ClutterInitError
ClutterQt::init (int *argc, char ***argv)
{
  g_type_init ();
  clutter_x11_set_display (QX11Info::display ());
  clutter_x11_disable_event_retrieval ();

  return clutter_init (argc, argv);
}

ClutterQt::ClutterQt (QWidget *parent)
  : QWidget (parent)
{
  priv = new ClutterQtPrivate;

  /* we always create new stages rather than use the default */
  priv->stage = clutter_stage_new ();

  priv->scrollPos = 0;
      
  /* we must realize the stage to get it ready for embedding */
  clutter_actor_realize (priv->stage);

  /* Avoid QT's painting system */
  setAttribute (Qt::WA_OpaquePaintEvent, true);
  setAttribute (Qt::WA_NoSystemBackground, true);
  setAttribute (Qt::WA_PaintOnScreen, true);

  setMouseTracking (true);

  resetWindow (parent);

  /* Reset the timer */
  priv->time.start ();
}

void
ClutterQt::resetWindow (QWidget *parent)
{
  XSetWindowAttributes xattr;
  unsigned long mask;
  const QX11Info &x11info = x11Info ();
  
  /* Create an X11 window using the visual and colormap that
     clutter expects */
  
  /* window attributes */  
  xattr.background_pixel = WhitePixel (QX11Info::display (),
				       x11info.screen ());
  xattr.border_pixel = 0;
  
  Qt::HANDLE parent_win;

  if (parent)
    parent_win = parent->winId();
  else
    parent_win = QX11Info::appRootWindow (x11info.screen ());

  const XVisualInfo *xvinfo
    = clutter_x11_get_stage_visual (CLUTTER_STAGE (priv->stage));
  
  xattr.colormap = XCreateColormap (QX11Info::display (),
				    parent_win,
				    xvinfo->visual,
				    AllocNone);
  mask = CWBackPixel | CWBorderPixel | CWColormap;
  WId win = XCreateWindow (QX11Info::display (),
			   parent_win,
			   0, 0, 1, 1,
			   0,
			   xvinfo->depth,
			   InputOutput,
			   xvinfo->visual,
			   mask, &xattr);

  clutter_x11_set_stage_foreign (CLUTTER_STAGE (priv->stage), win);
  clutter_actor_queue_redraw (CLUTTER_ACTOR (priv->stage));

  /* Replace the widget's window with the one we created */
  create (win, true, true);
}

ClutterQt::~ClutterQt ()
{
  if (priv->stage)
    clutter_actor_destroy (priv->stage);

  delete priv;
}

void
ClutterQt::resizeEvent (QResizeEvent *event)
{
  /* Chain up */
  QWidget::resizeEvent (event);

  /* Pass on the size to the stage */
  clutter_actor_set_size (priv->stage,
			  event->size ().width (),
			  event->size ().height ());
}

void
ClutterQt::changeEvent (QEvent *event)
{
  QWidget::changeEvent (event);

  if (event->type () == QEvent::ParentChange)
    resetWindow (parentWidget ());
}

void
ClutterQt::paintEvent (QPaintEvent *)
{
  if (CLUTTER_ACTOR_IS_VISIBLE (priv->stage))
    clutter_actor_queue_redraw (priv->stage);
}

void
ClutterQt::showEvent (QShowEvent *)
{
  clutter_actor_show (priv->stage);

  /* The backend wont get the XEvent so we have to make sure we set
     the event here.
   */
  CLUTTER_ACTOR_SET_FLAGS (priv->stage, CLUTTER_ACTOR_MAPPED);
}

void
ClutterQt::hideEvent (QHideEvent *)
{
  clutter_actor_hide (priv->stage);
}

ClutterModifierType
ClutterQt::getModifierState (QInputEvent *event)
{
  int ret = 0;
  Qt::KeyboardModifiers qmods = event->modifiers ();

  if ((qmods & Qt::ShiftModifier))
    ret |= CLUTTER_SHIFT_MASK;
  if ((qmods & Qt::ControlModifier))
    ret |= CLUTTER_CONTROL_MASK;
  if ((qmods & Qt::AltModifier))
    ret |= CLUTTER_MOD1_MASK;
  if ((qmods & Qt::MetaModifier))
    ret |= CLUTTER_MOD2_MASK;

  if (event->type() == QEvent::MouseButtonPress ||
      event->type() == QEvent::MouseButtonDblClick ||
      event->type() == QEvent::MouseButtonRelease ||
      event->type() == QEvent::MouseMove)
    {
      QMouseEvent *mouse_event = static_cast<QMouseEvent *> (event);
      Qt::MouseButtons buttons = mouse_event->buttons ();

      if ((buttons & Qt::LeftButton))
	ret |= CLUTTER_BUTTON1_MASK;
      if ((buttons & Qt::MidButton))
	ret |= CLUTTER_BUTTON2_MASK;
      if ((buttons & Qt::RightButton))
	ret |= CLUTTER_BUTTON3_MASK;
    }

  return (ClutterModifierType) ret;
}

void
ClutterQt::mouseMoveEvent (QMouseEvent *event)
{
  ClutterEvent cevent;

  memset (&cevent, 0, sizeof (cevent));

  cevent.type = CLUTTER_MOTION;
  cevent.any.stage = CLUTTER_STAGE (priv->stage);
  cevent.motion.x = event->x ();
  cevent.motion.y = event->y ();
  cevent.motion.time = priv->time.elapsed ();
  cevent.motion.modifier_state = getModifierState (event);

  clutter_do_event (&cevent);

  /* doh - motion events can push ENTER/LEAVE events onto Clutters
   * internal event queue which we do really ever touch (essentially
   * proxying from QT's queue). The below pumps them back out and
   * processes.
   * *could* be side effects with below though doubful as no other
   * events reach the queue (we shut down event collection). Maybe
   * a peek_mask type call could be even safer. 
  */
  while (clutter_events_pending())
    {
      ClutterEvent *ev = clutter_event_get ();
      if (ev)
        {
          clutter_do_event (ev);
          clutter_event_free (ev);
        }
    }
}

void
ClutterQt::mousePressOrReleaseEvent (QMouseEvent *event)
{
  ClutterEvent cevent;

  memset (&cevent, 0, sizeof (cevent));

  cevent.type = event->type () == QEvent::MouseButtonPress
    ? CLUTTER_BUTTON_PRESS : CLUTTER_BUTTON_RELEASE;

  cevent.button.button
    = event->button () == Qt::LeftButton ? 1
    : event->button () == Qt::MidButton ? 2
    : event->button () == Qt::RightButton ? 3
    : event->button ();

  cevent.any.stage = CLUTTER_STAGE (priv->stage);
  cevent.button.x = event->x ();
  cevent.button.y = event->y ();
  cevent.button.time = priv->time.elapsed ();
  cevent.button.click_count = 1;
  cevent.motion.modifier_state = getModifierState (event);

  clutter_do_event (&cevent);
}

void
ClutterQt::mousePressEvent (QMouseEvent *event)
{
  mousePressOrReleaseEvent (event);
}

void
ClutterQt::mouseReleaseEvent (QMouseEvent *event)
{
  mousePressOrReleaseEvent (event);
}

void
ClutterQt::keyPressOrReleaseEvent (QKeyEvent *event)
{
  ClutterEvent cevent;

  memset (&cevent, 0, sizeof (cevent));

  if (event->type () == QEvent::KeyPress)
    cevent.type = cevent.key.type = CLUTTER_KEY_PRESS;
  else if (event->type () == QEvent::KeyRelease)
    cevent.type = cevent.key.type = CLUTTER_KEY_RELEASE;
  else
    return;

  cevent.any.stage = CLUTTER_STAGE (priv->stage);
  cevent.key.time = priv->time.elapsed ();
  cevent.key.modifier_state = getModifierState (event);
  cevent.key.keyval = event->nativeVirtualKey ();
  cevent.key.hardware_keycode = event->nativeScanCode ();
  if (event->text ().size () > 0)
    cevent.key.unicode_value = event->text ()[0].unicode ();

  clutter_do_event (&cevent);
}

void
ClutterQt::keyPressEvent (QKeyEvent *event)
{
  keyPressOrReleaseEvent (event);
}

void
ClutterQt::keyReleaseEvent (QKeyEvent *event)
{
  keyPressOrReleaseEvent (event);
}

void
ClutterQt::wheelEvent (QWheelEvent *event)
{
  ClutterEvent cevent;

  /* Accumulate scroll changes until we get enough to count as a
     Clutter event */
  priv->scrollPos += event->delta ();

  if (ABS (priv->scrollPos) >= MIN_WHEEL_DELTA)
    {
      memset (&cevent, 0, sizeof (cevent));
      
      cevent.type = CLUTTER_SCROLL;
      cevent.any.stage = CLUTTER_STAGE (priv->stage);
      cevent.scroll.x = event->x ();
      cevent.scroll.y = event->y ();
      cevent.scroll.time = priv->time.elapsed ();
      cevent.scroll.modifier_state = getModifierState (event);

      if (priv->scrollPos > 0)
	{
	  cevent.scroll.direction = CLUTTER_SCROLL_UP;
	  priv->scrollPos -= MIN_WHEEL_DELTA;
	}
      else
	{
	  cevent.scroll.direction = CLUTTER_SCROLL_DOWN;
	  priv->scrollPos += MIN_WHEEL_DELTA;
	}

      clutter_do_event (&cevent);
    }
}

ClutterActor *
ClutterQt::stage () const
{
  return priv->stage;
}
