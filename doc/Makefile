DISTCLEANFILES = *.info* *.ps *.html

CLEANFILES = *.log *.dvi *.aux *.dlog \
             *.cp *.cps *.fn *.ky *.pg *.toc *.tp *.vr


info:
	makeinfo gnushogi.texinfo

tex:
	texi2dvi gnushogi.texinfo

# I do some egregious hacks to get around limitations of 
# texi2html.

html:
	texi2html -split_node gnushogi.texinfo
	./make_fig1 < gnushogi_6.html > tmp.html
	mv tmp.html gnushogi_6.html
	./make_fig2 < gnushogi_14.html > tmp.html
	mv tmp.html gnushogi_14.html

view:
	xdvi gnushogi.dvi

ps:
	dvips -t letter gnushogi.dvi -o gnushogi.ps

ps2:
	dvi2ps gnushogi.dvi > gnushogi.ps

spell:
	tr '[A-Z]' '[a-z]' < gnushogi.texinfo | tr -cd '[A-Za-z0-9_ \012]' | \
	tr -s '[ ]' '\012' | sort | uniq -c | sort -nr | spell

clean:
	@for file in $(CLEANFILES); \
        do if [ -f $$file ]; then rm $$file; fi; \
        done

distclean: clean
	@for file in $(DISTCLEANFILES); \
        do if [ -f $$file ]; then rm $$file; fi; \
        done



