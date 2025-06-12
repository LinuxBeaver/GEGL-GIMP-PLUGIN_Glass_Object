/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * 2024, beaver, Glass Object

gaussian-blur std-dev-x=3 std-dev-y=3
lb:metallic solar1=1.5 solar2=4.8 solar3=2.1
color-to-alpha transparency-threshold=0.079 
opacity-threshold=1.0 color=#6a6a6a
id=1 over aux=[ ref=1 ] 
gaussian-blur std-dev-x=0.2 std-dev-y=0.2


]

end of syntax
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double (smooth, _("Smooth Glass"), 0.5)
  value_range   (0.0, 3.0)
  ui_range      (0.0, 3.0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")

property_double (hyperopacity, _("Full Opacity of Glass object"), 1.0)
  value_range   (0.0, 1.4)
  ui_range      (0.0, 1.4)


property_double (glassrotate1, _("Glass light rotation (red channel)"), 0.130)
    description (_("Alien map color shift of the red channel to make a glass effect "))
  value_range (0.0, 0.6)
  ui_steps      (0.100, 1.00)



property_double  (glassrotate2, _("Glass light rotation (green channel)"), 4.8)
    description (_("Alien map color shift of the green channel to make a glass effect "))
  value_range (3.5, 6)
  ui_steps      (0.100, 1.00)


property_double  (glassrotate3, _("Glass light rotation (blue channel)"), 2.1)
    description (_("Alien map color shift of the blue channel to make a glass effect "))
  value_range (1.5, 3.2)
  ui_steps      (0.100, 1.00)


property_double (x, _("X Shadow/Outline"), 0.0)
  description   (_("Horizontal shadow offset"))
  ui_range      (-40.0, 40.0)
  ui_steps      (1, 10)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "x")


property_double (y, _("Y Shadow/Outline "), 0.0)
  description   (_("Vertical shadow offset"))
  ui_range      (-40.0, 40.0)
  ui_steps      (1, 10)
  ui_meta       ("unit", "pixel-distance")
  ui_meta       ("axis", "y")


property_double (radius, _("Shadow/Outline Blur radius"), 5.0)
  value_range   (0.0, G_MAXDOUBLE)
  ui_range      (0.0, 50.0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")

property_double (grow_radius, _("Shadow/Outline Grow radius"), -1.0)
  value_range   (-100.0, 100.0)
  ui_range      (-20.0, 20.0)
  ui_digits     (0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")
  description (_("The distance to expand the shadow; a negative value will contract the shadow instead"))

property_double (opacity, _("Opacity of Shadow/Outline"), 0.5)
  value_range   (0.0, 2.0)
  ui_range      (0.0, 2.0)
  ui_steps      (1, 5)
  ui_gamma      (1.5)
  ui_meta       ("unit", "pixel-distance")

property_color  (color, _("Color of shadow/outline"), "black")
    /* TRANSLATORS: the string 'black' should not be translated */
  description   (_("The shadow's color (defaults to 'black')"))

#else

#define GEGL_OP_META
#define GEGL_OP_NAME     glassman
#define GEGL_OP_C_SOURCE glassman.c

#include "gegl-op.h"

/*starred nodes go inside typedef struct */

typedef struct
{
 GeglNode *input;
 GeglNode *output;
 GeglNode *metallic;
 GeglNode *smooth;
 GeglNode *dropshadow;
 GeglNode *string;
 GeglNode *hyperopacity;
 GeglNode *c2a;
}State;

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglProperties *o = GEGL_PROPERTIES (operation);
  GeglColor *grey = gegl_color_new ("#6a6a6a");

  State *state = o->user_data = g_malloc0 (sizeof (State));

/*new child node list is here, this is where starred nodes get defined

 state->newchildname = gegl_node_new_child (gegl, "operation", "lb:name", NULL);*/
  state->input    = gegl_node_get_input_proxy (gegl, "input");
  state->output   = gegl_node_get_output_proxy (gegl, "output");

 state->smooth = gegl_node_new_child (gegl, "operation", "gegl:gaussian-blur", "clip-extent", FALSE, "abyss-policy", 0, NULL);

 state->dropshadow = gegl_node_new_child (gegl, "operation", "gegl:dropshadow", NULL);

 state->metallic = gegl_node_new_child (gegl, "operation", "lb:metallic", NULL);

 state->hyperopacity = gegl_node_new_child (gegl, "operation", "gegl:opacity", NULL);

 state->c2a = gegl_node_new_child (gegl, "operation", "gegl:color-to-alpha", "color", grey, "transparency-threshold", 0.079, "opacity-threshold", 1.0,  NULL);

#define glassstring \
" id=1 over aux=[ ref=1 ]  gaussian-blur std-dev-x=0.2 std-dev-y=0.2 clip-extent=false motion-blur-linear length=0.5 "\

 state->string = gegl_node_new_child (gegl, "operation", "gegl:gegl", "string", glassstring, NULL);
/*meta redirect property to new child orders go here
 * 



 gegl_operation_meta_redirect (operation, "propertyname", state->newchildname,  "originalpropertyname");
*/

}

static void
update_graph (GeglOperation *operation)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  State *state = o->user_data;
  if (!state) return;

  gegl_node_link_many (state->input, state->smooth, state->metallic, state->dropshadow, state->c2a, state->string, state->hyperopacity,  state->output,  NULL);

  gegl_operation_meta_redirect (operation, "smooth", state->smooth, "std-dev-y");
  gegl_operation_meta_redirect (operation, "smooth", state->smooth, "std-dev-x");
  gegl_operation_meta_redirect (operation, "glassrotate1", state->metallic, "solar1");
  gegl_operation_meta_redirect (operation, "glassrotate2", state->metallic, "solar2");
  gegl_operation_meta_redirect (operation, "glassrotate3", state->metallic, "solar3");
  gegl_operation_meta_redirect (operation, "color", state->dropshadow, "color");
  gegl_operation_meta_redirect (operation, "grow-radius", state->dropshadow, "grow-radius");
  gegl_operation_meta_redirect (operation, "radius", state->dropshadow, "radius");
  gegl_operation_meta_redirect (operation, "opacity", state->dropshadow, "opacity");
  gegl_operation_meta_redirect (operation, "hyperopacity", state->hyperopacity, "value");
  gegl_operation_meta_redirect (operation, "y", state->dropshadow, "y");
  gegl_operation_meta_redirect (operation, "x", state->dropshadow, "x");

}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;
GeglOperationMetaClass *operation_meta_class = GEGL_OPERATION_META_CLASS (klass);
  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;
  operation_meta_class->update = update_graph;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:glassobject",
    "title",       _("Glass Object"),
    "reference-hash", "ivotedfornobodybecauseagorism",
    "description", _("Turn subject/object into Glass with the addition of a shadow/outline because those effects do not apply normally on glass"),
/*<Image>/Colors <Image>/Filters are top level menus in GIMP*/
    "gimp:menu-path", "<Image>/Filters/Artistic",
    "gimp:menu-label", _("Glass transformation..."),
    NULL);
}

#endif
