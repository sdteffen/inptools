<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:import href=
    "../../docbook-xsl/html/profile-chunk.xsl"/>
  <xsl:import href="docbook_general.xsl"></xsl:import>
  <xsl:param name="admon.graphics">1</xsl:param>
  <xsl:param name="callout.graphics">1</xsl:param>

<xsl:param name="admon.graphics" select="1" />

<xsl:param name="generate.toc">
appendix  toc,title
article/appendix  nop
article   toc,title
book      toc,title,figure,table,example,equation
chapter   nop
part      toc,title
preface   toc,title
qandadiv  toc
qandaset  toc
reference toc,title
sect1     toc
sect2     toc
sect3     toc
sect4     toc
sect5     toc
section   toc
set       toc,title
</xsl:param>

<xsl:param name="chunk.section.depth">0</xsl:param>

<xsl:template name="user.header.content">
  <p><a href="http://epanet.de">Epanet.de</a> / <a href="../index.html">InpTools</a> / Manual</p>
  <script type="text/javascript">
window.google_analytics_uacct = "UA-389797-7";  
google_ad_client = "pub-6885897094183387";
google_ad_width = 728;
google_ad_height = 90;
google_ad_format = "728x90_as";
google_ad_type = "text_image";
google_ad_channel = "";
google_color_border = "000000";
google_color_bg = "F0F0F0";
google_color_link = "0000FF";
google_color_text = "000000";
google_color_url = "008000";
</script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js"/>

  
</xsl:template>

<xsl:template name="user.footer.content">
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js" />
  <p><a href="http://epanet.de">Epanet.de</a> / <a href="../index.html">InpTools</a> / Manual</p>
  <p><a href="http://epanet.de/en/imprint.html">Imprint</a> | 
  	<a href="http://epanet.de/de/imprint.html">Impressum</a> |
  	<a href="http://epanet.de/en/privacypolicy.html">Privacy Policy</a> |
  	<a href="http://epanet.de/de/datenschutzerklaerung.html">Datenschutzerklärung</a></p>
<script type="text/javascript">
var gaJsHost = (("https:" == document.location.protocol) ? "https://ssl." : "http://www.");
document.write(unescape("%3Cscript src='" + gaJsHost + "google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E"));
</script>
<script type="text/javascript">
var pageTracker = _gat._getTracker("UA-389797-7");
pageTracker._initData();
pageTracker._trackPageview();
</script>
</xsl:template>

</xsl:stylesheet>