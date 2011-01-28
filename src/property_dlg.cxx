#include "property_dlg.h"
#include "fsvwindow.h"

/* Creates the clist widget used in the "Contents" page of the Properties
 * dialog for a directory */
struct PropColumns : public Gtk::TreeModelColumnRecord{
  Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > icon;
  Gtk::TreeModelColumn<Glib::ustring> name;
  Gtk::TreeModelColumn<unsigned int> count;
  Gtk::TreeModelColumn<unsigned int> size;
  PropColumns(){
    add(icon);
    add(name);
    add(count);
    add(size);
  }
};

void PropertyDialog::make_dircontent(GNode *dnode)
{
	PropColumns prop_cols;
  Glib::RefPtr<Gtk::ListStore> model = Gtk::ListStore::create(prop_cols);
	dircontent = Gtk::manage( new Gtk::TreeView(model));

	g_assert( NODE_IS_DIR(dnode) );

	Gtk::TreeView::Column* pcol = Gtk::manage( new Gtk::TreeView::Column(_("Node type") ) );
	pcol->pack_start(prop_cols.icon,false);
	pcol->pack_start(prop_cols.name);
	dircontent->append_column(*pcol);
	pcol = Gtk::manage( new Gtk::TreeView::Column(_("Quantity"),prop_cols.count ) );
  dircontent->append_column(*pcol);

	for (int i = 1; i < NUM_NODE_TYPES; i++) {
	  Gtk::TreeRow row = *(model->append());
	  row[prop_cols.icon] = FsvWindow::node_type_mini_icons[i];
	  row[prop_cols.name] = _(node_type_plural_names[i]);
	  row[prop_cols.count] = DIR_NODE_DESC(dnode)->subtree.counts[i];
	}
}



PropertyDialog::PropertyDialog(GNode *nd ): Gtk::Dialog(_("Properties"),true),node(nd)
  ,tbl_gen(6,2)
  ,lbl_type(_("Type:"),0.9,0.5)
  ,lbl_loc(_("Location:"),0.9,0.5)
  ,lbl_alloc(_("Allocation:"),0.9,0.5)
  ,lbl_owner(_("Owner:"),0.9,0.5)
  ,lbl_group(_("Group:"),0.9,0.5)
  ,lbl_modif(_("Modified:"),0.9,0.5)
  ,lbl_attrib(_("AttribCh:"),0.9,0.5)
  ,lbl_access( _("Accessed:"),0.9,0.5)
{
  get_vbox()->add(nbk);
  nbk.append_page(tbl_gen,_("General"));
  tbl_gen.set_spacings(5);
  tbl_gen.set_border_width(10);
  const struct NodeInfo* node_info = get_node_info( node );
  if(NODE_DESC(node)->type==NODE_DIRECTORY){
    ico.set(Gtk::Stock::DIRECTORY,Gtk::IconSize(Gtk::ICON_SIZE_DIALOG) );
  } else if(NODE_DESC(node)->type==NODE_REGFILE){
    ico.set(Gtk::Stock::FILE,Gtk::IconSize(Gtk::ICON_SIZE_DIALOG) );
  } else {
    ico.set(Gdk::Pixbuf::create_from_xpm_data(node_type_xpms[NODE_DESC(node)->type]));
  }
  int pos=0; 
  tbl_gen.attach(ico,0,1,pos,pos+1);
  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(node_info->name,0.,0.5));
  tbl_gen.attach(*lbl,1,2,pos,pos+1);
  ++pos;
  tbl_gen.attach(lbl_type,0,1,pos,pos+1);
  lbl = Gtk::manage(new Gtk::Label(_(node_type_names[NODE_DESC(node)->type]),0.,0.5));
  tbl_gen.attach(*lbl,1,2,pos,pos+1);
  ++pos;
  tbl_gen.attach(lbl_loc,0,1,pos,pos+1);
  lbl = Gtk::manage(new Gtk::Label(node_info->prefix,0.,0.5));
  tbl_gen.attach(*lbl,1,2,pos,pos+1);
  ++pos;
	if (NODE_IS_DIR(node)){
    lbl_size.set_label(_("Total size:"));
    tbl_gen.attach(lbl_size,0,1,pos,pos+1);
    Glib::ustring lbsz(node_info->subtree_size);
    lbsz+=" bytes";
    if (DIR_NODE_DESC(node)->subtree.size >= 1024) {
			lbsz+=Glib::ustring(" (")+node_info->subtree_size_abbr+")";
		}
    lbl = Gtk::manage(new Gtk::Label(lbsz,0.,0.5));
    tbl_gen.attach(*lbl,1,2,pos,pos+1);
	} else {
    lbl_size.set_label(_("Size:"));
    tbl_gen.attach(lbl_size,0,1,pos,pos+1);
    lbl = Gtk::manage(new Gtk::Label(Glib::ustring(node_info->size)+" bytes ("+
                                     node_info->size_abbr+")",0.,0.5));
    tbl_gen.attach(*lbl,1,2,pos,pos+1);
    ++pos;
    tbl_gen.attach(lbl_alloc,0,1,pos,pos+1);
    lbl = Gtk::manage(new Gtk::Label(node_info->size_alloc,0.,0.5));
    tbl_gen.attach(*lbl,1,2,pos,pos+1);
	}
	++pos;
  tbl_gen.attach(lbl_owner,0,1,pos,pos+1);
  std::stringstream lbow;
  lbow<<node_info->user_name<<" (uid "<<NODE_DESC(node)->user_id<<")";
  lbl = Gtk::manage(new Gtk::Label(lbow.str(),0.,0.5));
  tbl_gen.attach(*lbl,1,2,pos,pos+1);
  ++pos;
  tbl_gen.attach(lbl_group,0,1,pos,pos+1);
  std::stringstream lbgr;
  lbgr<<node_info->group_name<<" (gid "<<NODE_DESC(node)->group_id<<")";
  lbl = Gtk::manage(new Gtk::Label(lbgr.str(),0.,0.5));
  tbl_gen.attach(*lbl,1,2,pos,pos+1);
  ++pos;
  
  tbl_gen.attach(lbl_modif,0,1,pos,pos+1);
  lbl = Gtk::manage(new Gtk::Label(node_info->mtime,0.,0.5));
  tbl_gen.attach(*lbl,1,2,pos,pos+1);
  ++pos;
  tbl_gen.attach(lbl_attrib,0,1,pos,pos+1);
  lbl = Gtk::manage(new Gtk::Label(node_info->ctime,0.,0.5));
  tbl_gen.attach(*lbl,1,2,pos,pos+1);
  ++pos;
  tbl_gen.attach(lbl_access,0,1,pos,pos+1);
  lbl = Gtk::manage(new Gtk::Label(node_info->atime,0.,0.5));
  tbl_gen.attach(*lbl,1,2,pos,pos+1);
  ++pos;
  add_button(Gtk::Stock::CLOSE, Gtk::RESPONSE_CANCEL);
  
	switch (NODE_DESC(node)->type) {
		case NODE_DIRECTORY:{
      nbk.append_page(tbl_additional,_("Contents"));
      tbl_additional.set_spacings(5);
      tbl_additional.set_border_width(10);
		  Gtk::Label *lb = Gtk::manage(new Gtk::Label(_("This directory contains:")));
		  
	    tbl_additional.attach(*lb,0,1,0,1);
      make_dircontent(node);
	    tbl_additional.attach(*dircontent,0,1,1,2);
		}
    break;
    case NODE_REGFILE:{
      nbk.append_page(tbl_additional,_("File type"));
      tbl_additional.set_spacings(5);
      tbl_additional.set_border_width(10);
#ifdef HAVE_FILE_COMMAND
		  Gtk::Label *lb = Gtk::manage(new Gtk::Label(_("This file is recognized as:")));
	    tbl_additional.attach(*lb,0,1,0,1);
      Gtk::ScrolledWindow *scr = Gtk::manage(new Gtk::ScrolledWindow());
      Gtk::TextView * ent = Gtk::manage(new Gtk::TextView());
      ent->get_buffer()->set_text(node_info->file_type_desc);
      ent->set_size_request(200,-1);
      ent->set_wrap_mode(Gtk::WRAP_WORD);
      scr->add(*ent);
	    tbl_additional.attach(*scr,0,1,1,2);
#endif //HAVE_FILE_COMMAND
    }
    break;
    case NODE_SYMLINK:{
      nbk.append_page(tbl_additional,_("Target"));
      tbl_additional.set_spacings(5);
      tbl_additional.set_border_width(10);
		  Gtk::Label *lb = Gtk::manage(new Gtk::Label(_("This symlink points to:")));
	    tbl_additional.attach(*lb,0,1,0,1);
      lb = Gtk::manage(new Gtk::Label(node_info->target));
	    tbl_additional.attach(*lb,0,1,1,2);
    }
    break;
  }
  show_all_children();
}

