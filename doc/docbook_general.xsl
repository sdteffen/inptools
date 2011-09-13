<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:param name="generate.index">1</xsl:param>
  <xsl:param name="preface.autolabel">0</xsl:param>
  <xsl:param name="part.autolabel">1</xsl:param>
  <xsl:param name="label.from.part">1</xsl:param>
  <xsl:param name="chapter.autolabel">1</xsl:param>
  <xsl:param name="section.autolabel">1</xsl:param>
  <xsl:param name="qandadiv.autolabel">1</xsl:param>
  <xsl:param name="generate.section.toc.level">2</xsl:param>
  <xsl:param name="toc.section.depth">3</xsl:param>
  <xsl:param name="suppress.header.navigation">1</xsl:param>
  <xsl:param name="header.rule">1</xsl:param>
  <xsl:param name="suppress.footer.navigation">0</xsl:param>
  <xsl:param name="footer.rule">1</xsl:param>
  <xsl:param name="html.stylesheet">docbook.css</xsl:param>
  <xsl:param name="html.stylesheet.type">text/css</xsl:param>
  <xsl:param name="chunk.section.depth">3</xsl:param>
  <xsl:param name="html.ext">.html</xsl:param>
  <xsl:param name="chunk.quietly">1</xsl:param>
  <xsl:param name="base.dir"></xsl:param>
  <xsl:param name="use.id.as.filename">1</xsl:param>
  <xsl:param name="section.label.includes.component.label">1</xsl:param>
  <xsl:param name="chunker.output.indent">yes</xsl:param>
  <xsl:param name="chunk.first.sections">1</xsl:param>
  <xsl:param name="chunker.output.method">html</xsl:param>
  <xsl:param name="profile.separator">;</xsl:param>
  <xsl:param name="profile.arch"></xsl:param>
  <xsl:param name="profile.condition"></xsl:param>
  <xsl:param name="profile.conformance"></xsl:param>
  <xsl:param name="profile.lang"></xsl:param>
  <xsl:param name="profile.os"></xsl:param>
  <xsl:param name="profile.revision"></xsl:param>
  <xsl:param name="profile.revisionflag"></xsl:param>
  <xsl:param name="profile.role"></xsl:param>
  <xsl:param name="profile.security"></xsl:param>
  <xsl:param name="profile.userlevel"></xsl:param>
  <xsl:param name="profile.vendor"></xsl:param>
  <xsl:param name="profile.attribute"></xsl:param>
  <xsl:param name="profile.value"></xsl:param>
  <xsl:param name="formal.title.placement">
  figure after
  example before
  equation after
  table after
  procedure after
  </xsl:param>
  <xsl:param name="headers.on.blank.pages">0</xsl:param>
  <xsl:param name="menuchoice.menu.separator">-&gt;</xsl:param>
  <xsl:param name="firstterm.only.link">0</xsl:param>
  <xsl:param name="make.valid.html">1</xsl:param>
  <xsl:param name="html.cleanup">0</xsl:param>
  <xsl:param name="annotate.toc">0</xsl:param>
  <xsl:param name="autotoc.label.separator">.&#160;</xsl:param>
  <xsl:param name="callout.list.table">0</xsl:param>
  <xsl:param name="draft.mode" select="'no'"/>
  <xsl:param name="use.svg" select="0"/>
  <xsl:param name="tablecolumns.extension" select="'1'"/>
  <xsl:param name="biblioentry.item.separator">. </xsl:param>
  <xsl:param name="bibliography.numbered">1</xsl:param>
  <xsl:param name="css.decoration">1</xsl:param>
  <xsl:param name="html.longdesc">0</xsl:param>
  <xsl:param name="draft.watermark.image">images/draft.png</xsl:param>
  <xsl:param name="spacing.paras">0</xsl:param>
  <xsl:param name="segmentedlist.as.table">0</xsl:param>
  <xsl:param name="variablelist.as.table">0</xsl:param>
  <xsl:param name="variablelist.as.blocks">0</xsl:param>
  <xsl:param name="glossterm.auto.link">1</xsl:param>
  <xsl:param name="generate.legalnotice.link" select="0"/>
  <xsl:param name="admon.graphics">1</xsl:param>
  <xsl:param name="admon.graphics.extension">.png</xsl:param>
  <xsl:param name="admon.graphics.path">images/</xsl:param>
  <xsl:param name="admon.style"></xsl:param>
  <xsl:param name="page.margin.inner">2.4cm</xsl:param>
  <xsl:param name="page.margin.outer">2.4cm</xsl:param>
  <xsl:param name="paper.type">A4</xsl:param>
  <xsl:param name="page.orientation">portrait</xsl:param>
  <xsl:param name="double.sided">0</xsl:param>
  <xsl:param name="hyphenate">true</xsl:param>
  <xsl:param name="alignment">justify</xsl:param>
  <xsl:param name="column.count.body">1</xsl:param>
  <xsl:param name="body.font.family">Times</xsl:param>
  <xsl:param name="title.margin.left">0cm</xsl:param>
  <xsl:param name="toc.indent.width">20</xsl:param>
  <xsl:param name="ulink.show">0</xsl:param>
  <xsl:param name="insert.xref.page.number">0</xsl:param>
</xsl:stylesheet>
