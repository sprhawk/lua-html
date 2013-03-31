#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>

#include <libxml/HTMLParser.h>

#define HTML_DOCUMENT_METATABLE_TYPE "HTML.document"
#define HTML_NODE_METATABLE_TYPE "HTML.node"

typedef struct _html_document {
  htmlDocPtr doc;
} html_document, * html_document_ptr;

typedef struct _html_node {
  htmlNodePtr node;
  int deep;
}html_node, * html_node_ptr;

extern int html_new_document(lua_State * L);

int html_document_free(lua_State * L);
int html_node_free(lua_State * L);
int html_document_get_element_by_id(lua_State * L);
int html_node_has_attribute(lua_State * L);
int html_node_get_attribute(lua_State * L);
int html_node_deep_copy_from_node(lua_State * L);
int html_node_make_reference_from_node(lua_State * L);
int html_node_get_name(lua_State * L);
int html_node_get_type(lua_State * L);
int html_node_get_first_child(lua_State * L);
int html_node_get_next_sibling(lua_State * L);
int html_node_get_parent(lua_State * L);
int html_node_traverse_children(lua_State * L);

int luaopen_html(lua_State * L);

static html_node_ptr copy_html_node_from_node(lua_State * L, htmlNodePtr node);

void lerror(lua_State * L, const char * msg);

static const luaL_Reg reg[] = {
    {"newdocument", html_new_document},
    NULL, NULL,
  };


static const luaL_Reg html_document_methods[] = {
  "__gc", html_document_free,
  "getElementById", html_document_get_element_by_id,
  NULL, NULL,
};

static const luaL_Reg html_node_methods[] = {
  "__gc", html_node_free,
  "copy", html_node_deep_copy_from_node,
  "makeRef", html_node_make_reference_from_node,
  "hasAttribute", html_node_has_attribute,
  "getAttribute", html_node_get_attribute,
  "name", html_node_get_name,
  "type", html_node_get_type,
  "firstChild", html_node_get_first_child,
  "nextSibling", html_node_get_next_sibling,
  "parent", html_node_get_parent,
  "traverse", html_node_traverse_children,
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

static html_node_ptr make_html_node_reference_from_node(lua_State * L, htmlNodePtr node) {
  html_node_ptr htmlnode;
  if(node) {
    htmlnode = (html_node_ptr)lua_newuserdata(L, sizeof(html_node));
    if(htmlnode) {
      htmlnode->node = node;
      htmlnode->deep = 0;
      int ret = luaL_newmetatable(L, HTML_NODE_METATABLE_TYPE);
      if(ret) {
        luaL_setfuncs(L, html_node_methods, 0);
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
      }
      lua_setmetatable(L, -2);
    }
    return htmlnode;
  }
}

static html_node_ptr deep_copy_html_node_from_node(lua_State * L, htmlNodePtr node)
{
  html_node_ptr htmlnode = NULL;
  htmlNodePtr newNode = xmlCopyNode(node, 1);
  if(newNode) {
    htmlnode = (html_node_ptr)lua_newuserdata(L, sizeof(html_node));
    if(htmlnode) {
      htmlnode->node = newNode;
      htmlnode->deep = 1;
      int ret = luaL_newmetatable(L, HTML_NODE_METATABLE_TYPE);
      if(ret) {
        luaL_setfuncs(L, html_node_methods, 0);
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
      }
      lua_setmetatable(L, -2);
    }
  }

  return htmlnode;
}

int html_node_get_name(lua_State * L) {
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, -1, HTML_NODE_METATABLE_TYPE);
  if(html_node && html_node->node) {
    xmlNodePtr node = html_node->node;
    const char * name = (const char *)node->name;
    if(name) {
      lua_pushstring(L, name);
    }
    else {
      lua_pushstring(L, "");
    }
    return 1;
  }
  
  lua_pushnil(L);
  return 1;

}

int html_node_get_type(lua_State * L) {
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, -1, HTML_NODE_METATABLE_TYPE);
  if(html_node && html_node->node) {
    xmlNodePtr node = html_node->node;
    unsigned int type = node->type;
    lua_pushunsigned(L, type);
    return 1;
  }
  
  lua_pushnil(L);
  return 1;

}

int html_node_get_first_child(lua_State * L) {
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, -1, HTML_NODE_METATABLE_TYPE);
  if(html_node && html_node->node) {
    xmlNodePtr node = html_node->node->children;
    if(node) {
      html_node_ptr child = make_html_node_reference_from_node(L, node);
      if(child) {
        return 1;
      }
    }
  }
  
  lua_pushnil(L);
  return 1;
}
int html_node_get_next_sibling(lua_State * L) {
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, -1, HTML_NODE_METATABLE_TYPE);
  if(html_node && html_node->node) {
    xmlNodePtr node = html_node->node->next;
    if(node) {
      html_node_ptr next = make_html_node_reference_from_node(L, node);
      if(next) {
        return 1;
      }
    }
  }
  
  lua_pushnil(L);
  return 1;
}
int html_node_get_parent(lua_State * L) {
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, -1, HTML_NODE_METATABLE_TYPE);
  if(html_node && html_node->node) {
    xmlNodePtr node = html_node->node->parent;
    if(node) {
      html_node_ptr parent = make_html_node_reference_from_node(L,node);
      if(parent) {
        return 1;
      }
      lerror(L, "make_html_node_reference_from_node failed");
    }
  }
  
  lua_pushnil(L);
  return 1;
}

/* 
 * Element:traverse(function)
 */
int html_node_traverse_children(lua_State * L) {
  
  return 0;
}

int html_node_deep_copy_from_node(lua_State * L)
{
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, -1, HTML_NODE_METATABLE_TYPE);
  if(html_node) {
    deep_copy_html_node_from_node(L, html_node->node);
  }
  else {
    lua_pushnil(L);
    lerror(L, "parameter error");
  }
  return 1;
}

int html_node_make_reference_from_node(lua_State * L)
{
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, -1, HTML_NODE_METATABLE_TYPE);
  if(html_node) {
    make_html_node_reference_from_node(L, html_node->node);
  }
  else {
    lua_pushnil(L);
    lerror(L, "parameter error");
  }
  return 1;
}

/* 
 * Element:hasAttribute(attrname)
 */
int html_node_has_attribute(lua_State * L)
{
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, 1, HTML_NODE_METATABLE_TYPE);
  if(html_node) {
    const char * attrname = luaL_checkstring(L, 2);
    if(attrname) {
      xmlChar * xmlAttrName = xmlCharStrdup(attrname);
      if(xmlAttrName) {
        xmlAttrPtr attr = xmlHasProp(html_node->node, xmlAttrName);
        xmlFree(xmlAttrName);
        xmlAttrName = NULL;
        int b = 0;
        if(attr) {
          b = 1;
        }
        lua_pushboolean(L, b);
        return 1;
      }
      lerror(L, "xmlCharStrdup failed");
    }
    lerror(L, "wrong 2nd parameter");
  }
  return 0;
}
/*
 * Element:getAttribute(attrname)
 */
int html_node_get_attribute(lua_State * L)
{
  html_node_ptr html_node = (html_node_ptr)luaL_checkudata(L, 1, HTML_NODE_METATABLE_TYPE);
  if(html_node) {
    const char * attrname = luaL_checkstring(L, 2);
    if(attrname) {
      xmlChar * xmlAttrName = xmlCharStrdup(attrname);
      if(xmlAttrName) {
        xmlAttrPtr attr = xmlHasProp(html_node->node, xmlAttrName);
        free(xmlAttrName);
        xmlAttrName = NULL;
        if(attr) {
          const char * content = (const char *)attr->children->content;
          lua_pushstring(L, content);
          return 1;
        }
        lua_pushnil(L);
        return 1;
      }
      lerror(L, "xmlCharStrdup() failed");
    }
    lerror(L, "wrong 2nd parameter(string type)");
  }
  lerror(L, "wrong userdata:html_node");
  return 1;
}
/*
 * Document:getElementById(idname)
 */
int html_document_get_element_by_id(lua_State * L) {
  html_document_ptr htmldoc = (html_document_ptr)luaL_checkudata(L, 1, HTML_DOCUMENT_METATABLE_TYPE);
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
          xmlAttrPtr attr = xmlHasProp(node, BAD_CAST "id");
          if(attr) {
            int len = strlen(element_id);
            xmlChar * eid = xmlCharStrndup(element_id, len);
            int cmp = xmlStrcasecmp(eid, attr->children->content);
            /*printf("%s\n", BAD_CAST attr->children->content);*/
            if (0 == cmp) {
              printf("%s%s(id:%s)\n", &spaces[sizeof(spaces) - 1 - space],BAD_CAST node->name, BAD_CAST attr->children->content);
              /*html_node_ptr html_node = deep_copy_html_node_from_node(L, node);*/
              html_node_ptr html_node = make_html_node_reference_from_node(L, node);
              return 1;
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
        html_document_ptr htmldoc = (html_document_ptr)lua_newuserdata(L, sizeof(html_document));
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

int html_document_free(lua_State * L) {
  html_document_ptr html_ptr = (html_document_ptr)luaL_checkudata(L, 1, HTML_DOCUMENT_METATABLE_TYPE);
  if(html_ptr) {
    if(html_ptr->doc) {
      xmlFreeDoc(html_ptr->doc);
      html_ptr->doc = NULL;
    }
  }
  return 0;
}

int html_node_free(lua_State * L) {
  html_node_ptr node_ptr = (html_node_ptr)luaL_checkudata(L, 1, HTML_NODE_METATABLE_TYPE);
  if(node_ptr) {
    if(node_ptr->deep && node_ptr->node) {
      xmlFreeNode(node_ptr->node);
    }
    node_ptr->node = NULL;
    node_ptr->deep = 0;
  }
  return 0;
}

