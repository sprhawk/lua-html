#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>

#include <libxml/HTMLParser.h>

#define HTML_DOCUMENT_METATABLE_TYPE "HTML.document"

typedef struct _HTML {
  htmlDocPtr doc;
} HTML, * HTML_ptr;


extern int html_new_document(lua_State * L);

int html_free(lua_State * L);
int html_get_element_by_id(lua_State * L);
int luaopen_html(lua_State * L) ;

void lerror(lua_State * L, const char * msg);

static const luaL_Reg reg[] = {
    {"newdocument", html_new_document},
    NULL, NULL,
  };


static const luaL_Reg html_document_methods[] = {
  "__gc", html_free,
  "getElementById", html_get_element_by_id,
  NULL, NULL,
};


int luaopen_html(lua_State * L) {

  luaL_newlib(L, reg);
  lua_pushvalue(L, -1);
  lua_setglobal(L, "html");
  return 1;
}

void lerror(lua_State * L, const char * msg)
{
  lua_pushstring(L, msg);
  lua_error(L);
}

static xmlAttrPtr get_element_attribute_by_name(xmlNodePtr node, const xmlChar * name)
{
  xmlAttrPtr attr = NULL;
  if(node) {
    attr = xmlHasProp(node, name);
  }
  return attr;
}

/*
 * Document:getElementById(idname)
 */
int html_get_element_by_id(lua_State * L) {
  HTML_ptr htmldoc = (HTML_ptr)luaL_checkudata(L, 1, HTML_DOCUMENT_METATABLE_TYPE);
  if (htmldoc && htmldoc->doc) {
    const char * element_id = luaL_checkstring(L, 2);
    if(element_id) {
      /*printf("doc type:%d\n", htmldocument->doc->type);*/
      xmlNodePtr node = htmldoc->doc->children;
      char spaces[21] = {0};
      memset(spaces, ' ', sizeof(spaces) - 1);
      int space = 0;
      while(node) {
        if(XML_ELEMENT_NODE == node->type) {
          /*printf("%s%s", &spaces[sizeof(spaces) - 1 - space],BAD_CAST node->name);*/
          xmlAttrPtr attr = get_element_attribute_by_name(node, BAD_CAST "id");
          if(attr) {
            int len = strlen(element_id);
            xmlChar * eid = xmlCharStrndup(element_id, len);
            int cmp = xmlStrcasecmp(eid, attr->children->content);
            /*printf("%s\n", BAD_CAST attr->children->content);*/
            if (0 == cmp) {
              printf("%s%s(id:%s)\n", &spaces[sizeof(spaces) - 1 - space],BAD_CAST node->name, BAD_CAST attr->children->content);
              
            }
          }
          /*printf("\n");*/
        }

        /* travers nodes */
        if(node->children) {
          node = node->children;
          space ++;
        }
        else if(node->next) {
          node = node->next;
        }
        else {
          xmlNodePtr parent = node;
          do {
            parent = parent->parent;
            if(space > 0) {
              space --;
            }
            else {
              /*printf("space exceedes\n");*/
            }
          }while(NULL != parent && NULL == parent->next);

          if(NULL == parent) {
            node = NULL;
          }
          else {
            node = parent->next;
          }
        }
      }
      return 1;
    }
    else {
      lerror(L, "parameter is not a string");
    }
  }
  lerror(L, "first argument is not a userdata to html parser state");
  return 0;
}

/*
 * HTML.new_document(html_str)
 */
int html_new_document(lua_State * L) {
  size_t len = 0;
  const char * html = NULL;
  html = luaL_checklstring(L, -1, &len);
  if (html) {
    htmlParserCtxtPtr ctx = htmlNewParserCtxt();
    if(ctx) {
      htmlDocPtr doc = NULL;
      doc = htmlCtxtReadMemory(ctx, html, len, NULL, NULL, HTML_PARSE_RECOVER|HTML_PARSE_NODEFDTD|HTML_PARSE_NOERROR);
      htmlFreeParserCtxt(ctx);
      ctx = NULL;

      if(doc) {
        HTML_ptr htmldoc = (HTML_ptr)lua_newuserdata(L, sizeof(HTML));
        if(htmldoc) {
          htmldoc->doc = doc;

          int ret = luaL_newmetatable(L, HTML_DOCUMENT_METATABLE_TYPE);
          if(ret) {
            luaL_setfuncs(L, html_document_methods, 0);
            lua_pushvalue(L, -1);
            lua_setfield(L, -2, "__index");
          }
          lua_setmetatable(L, -2);
          
          return 1;
        }
        if(doc) {
          xmlFreeDoc(doc);
          doc = NULL;
        }

        lerror(L, "failed to call lua_newuserdata");
      }
    }
  }

  lerror(L, "failed to call htmlCtxtReadMemory");
  lua_pushnil(L);
  return 1;
}

int html_free(lua_State * L) {
  HTML_ptr html_ptr = (HTML_ptr)lua_touserdata(L, -1);
  if(html_ptr) {
    if(html_ptr->doc) {
      xmlFreeDoc(html_ptr->doc);
      html_ptr->doc = NULL;
    }
    /*
    if(state->ctx) {
      htmlFreeParserCtxt(state->ctx);
      state->ctx = NULL;
    }
    */
  }
  return 0;
}

