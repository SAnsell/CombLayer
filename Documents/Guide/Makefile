Guide.pdf: Guide.tex
	@pdflatex $<
	@makeindex Guide.nlo -s nomencl.ist -o Guide.nls
	@bibtex Guide
	@pdflatex $<
