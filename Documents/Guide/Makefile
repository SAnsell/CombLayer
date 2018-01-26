.PHONY: clean

Guide.pdf: Guide.tex Guide.bib */*.tex
	@pdflatex $<
	@makeindex Guide.nlo -s nomencl.ist -o Guide.nls
	@bibtex Guide
	@pdflatex $<

clean:
	@rm -fv *.nl[os] *.[bi]lg *.bbl *.toc *.out *.aux *.log
