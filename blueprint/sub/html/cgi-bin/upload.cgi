#!/usr/bin/perl -w
use CGI;
use File::Spec;

# Get the upload directory from the environment variable, default to "html/upload"
$upload_dir = $ENV{'UPLOAD_DIR'} // "html/upload";

# Create a new CGI object
$query = new CGI;

# Check if the upload directory exists and is a directory
unless (-d $upload_dir) {
    print_error_and_exit("Upload directory not found or is not a directory: $upload_dir");
}

$filename = $query->param("photo");
$email_address = $query->param("email_address");

# Sanitize the filename
$filename =~ s/.*[\/\\](.*)/$1/;
$upload_filehandle = $query->upload("photo");

# Open the file for writing
$upload_path = File::Spec->catfile($upload_dir, $filename);
open UPLOADFILE, ">$upload_path" or die print_error_and_exit("Cannot open $upload_path: $!");

# Write the file to the specified directory
while (<$upload_filehandle>) {
    print UPLOADFILE;
}

close UPLOADFILE;

# Determine the URL path for the uploaded file
# Assuming the URL base path is the same as the filesystem path
# Adjust this if your URL structure is different
$upload_dir_url = $upload_dir;
$upload_dir_url =~ s/^html\///;

# Print the HTML response
print $query->header();
print <<END_HTML;
<HTML>
<HEAD>
<TITLE>Thanks!</TITLE>
</HEAD>
<BODY>
<P>Thanks for uploading your photo!</P>
<P>Your email address: $email_address</P>
<P>Your photo:</P>
<img src="/$upload_dir_url/$filename" border="0">
</BODY>
</HTML>
END_HTML

# Function to print an error message and exit
sub print_error_and_exit {
    my ($error_message) = @_;
    print $query->header();
    print <<END_HTML;
<HTML>
<HEAD>
<TITLE>Error</TITLE>
</HEAD>
<BODY>
<P><strong>Error:</strong> $error_message</P>
</BODY>
</HTML>
END_HTML
    exit;
}
------WebKitFormBoundarydTMbQf4Az7LD5RHQ
Content-Disposition: form-data; name="photo"; filename="img.png"
Content-Type: image/png

�PNG

   IHDR         ��   sRGB ���   �eXIfMM *                  J       R(       �i       Z      �     �    �       �       �           ]I
8   	pHYs , ,.
�;  YiTXtXML:com.adobe.xmp     <x:xmpmeta xmlns:x="adobe:ns:meta/" x:xmptk="XMP Core 5.4.0">
   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
      <rdf:Description rdf:about=""
            xmlns:tiff="http://ns.adobe.com/tiff/1.0/">
         <tiff:Orientation>1</tiff:Orientation>
      </rdf:Description>
   </rdf:RDF>
</x:xmpmeta>
L�'Y  IDAT8�ӱN1��v$qd��00�C�L���������Ӆ&^@7b⠃ˉ�w~-=���|��Ͽ��z��S����qY,��f���W����Y7���Q_�1���[\��HL������\E�ސB���b�Y��v�P,�*�c�{�a�1V�����	r|c�!�ц��v혘�R�蠅	zH��ؤ��p�������H9�iZZ]��]�ꦮ��<f�52��e
���
�~�>Q��;$�uӱNp�W��e�m�/���i�?��`k(����    IEND�B`�
------WebKitFormBoundarydTMbQf4Az7LD5RHQ
Content-Disposition: form-data; name="email_address"


------WebKitFormBoundarydTMbQf4Az7LD5RHQ
Content-Disposition: form-data; name="Submit"

Submit Form
------WebKitFormBoundarydTMbQf4Az7LD5RHQ--
------WebKitFormBoundary1heO5yHzBvcMSOnI
Content-Disposition: form-data; name="photo"; filename="img.png"
Content-Type: image/png

�PNG

   IHDR         ��   sRGB ���   �eXIfMM *                  J       R(       �i       Z      �     �    �       �       �           ]I
8   	pHYs , ,.
�;  YiTXtXML:com.adobe.xmp     <x:xmpmeta xmlns:x="adobe:ns:meta/" x:xmptk="XMP Core 5.4.0">
   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
      <rdf:Description rdf:about=""
            xmlns:tiff="http://ns.adobe.com/tiff/1.0/">
         <tiff:Orientation>1</tiff:Orientation>
      </rdf:Description>
   </rdf:RDF>
</x:xmpmeta>
L�'Y  IDAT8�ӱN1��v$qd��00�C�L���������Ӆ&^@7b⠃ˉ�w~-=���|��Ͽ��z��S����qY,��f���W����Y7���Q_�1���[\��HL������\E�ސB���b�Y��v�P,�*�c�{�a�1V�����	r|c�!�ц��v혘�R�蠅	zH��ؤ��p�������H9�iZZ]��]�ꦮ��<f�52��e
���
�~�>Q��;$�uӱNp�W��e�m�/���i�?��`k(����    IEND�B`�
------WebKitFormBoundary1heO5yHzBvcMSOnI
Content-Disposition: form-data; name="email_address"


------WebKitFormBoundary1heO5yHzBvcMSOnI
Content-Disposition: form-data; name="Submit"

Submit Form
------WebKitFormBoundary1heO5yHzBvcMSOnI--
------WebKitFormBoundaryRf7oZOBbTBxDhvxs
Content-Disposition: form-data; name="photo"; filename="img.png"
Content-Type: image/png

�PNG

   IHDR         ��   sRGB ���   �eXIfMM *                  J       R(       �i       Z      �     �    �       �       �           ]I
8   	pHYs , ,.
�;  YiTXtXML:com.adobe.xmp     <x:xmpmeta xmlns:x="adobe:ns:meta/" x:xmptk="XMP Core 5.4.0">
   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
      <rdf:Description rdf:about=""
            xmlns:tiff="http://ns.adobe.com/tiff/1.0/">
         <tiff:Orientation>1</tiff:Orientation>
      </rdf:Description>
   </rdf:RDF>
</x:xmpmeta>
L�'Y  IDAT8�ӱN1��v$qd��00�C�L���������Ӆ&^@7b⠃ˉ�w~-=���|��Ͽ��z��S����qY,��f���W����Y7���Q_�1���[\��HL������\E�ސB���b�Y��v�P,�*�c�{�a�1V�����	r|c�!�ц��v혘�R�蠅	zH��ؤ��p�������H9�iZZ]��]�ꦮ��<f�52��e
���
�~�>Q��;$�uӱNp�W��e�m�/���i�?��`k(����    IEND�B`�
------WebKitFormBoundaryRf7oZOBbTBxDhvxs
Content-Disposition: form-data; name="email_address"


------WebKitFormBoundaryRf7oZOBbTBxDhvxs
Content-Disposition: form-data; name="Submit"

Submit Form
------WebKitFormBoundaryRf7oZOBbTBxDhvxs--
------WebKitFormBoundarybmoKHzKtVCjDGPMB
Content-Disposition: form-data; name="photo"; filename="img.png"
Content-Type: image/png

�PNG

   IHDR         ��   sRGB ���   �eXIfMM *                  J       R(       �i       Z      �     �    �       �       �           ]I
8   	pHYs , ,.
�;  YiTXtXML:com.adobe.xmp     <x:xmpmeta xmlns:x="adobe:ns:meta/" x:xmptk="XMP Core 5.4.0">
   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
      <rdf:Description rdf:about=""
            xmlns:tiff="http://ns.adobe.com/tiff/1.0/">
         <tiff:Orientation>1</tiff:Orientation>
      </rdf:Description>
   </rdf:RDF>
</x:xmpmeta>
L�'Y  IDAT8�ӱN1��v$qd��00�C�L���������Ӆ&^@7b⠃ˉ�w~-=���|��Ͽ��z��S����qY,��f���W����Y7���Q_�1���[\��HL������\E�ސB���b�Y��v�P,�*�c�{�a�1V�����	r|c�!�ц��v혘�R�蠅	zH��ؤ��p�������H9�iZZ]��]�ꦮ��<f�52��e
���
�~�>Q��;$�uӱNp�W��e�m�/���i�?��`k(����    IEND�B`�
------WebKitFormBoundarybmoKHzKtVCjDGPMB
Content-Disposition: form-data; name="email_address"


------WebKitFormBoundarybmoKHzKtVCjDGPMB
Content-Disposition: form-data; name="Submit"

Submit Form
------WebKitFormBoundarybmoKHzKtVCjDGPMB--
------WebKitFormBoundaryuw0V7pwMnMgyV7Qr
Content-Disposition: form-data; name="photo"; filename="img.png"
Content-Type: image/png

�PNG

   IHDR         ��   sRGB ���   �eXIfMM *                  J       R(       �i       Z      �     �    �       �       �           ]I
8   	pHYs , ,.
�;  YiTXtXML:com.adobe.xmp     <x:xmpmeta xmlns:x="adobe:ns:meta/" x:xmptk="XMP Core 5.4.0">
   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
      <rdf:Description rdf:about=""
            xmlns:tiff="http://ns.adobe.com/tiff/1.0/">
         <tiff:Orientation>1</tiff:Orientation>
      </rdf:Description>
   </rdf:RDF>
</x:xmpmeta>
L�'Y  IDAT8�ӱN1��v$qd��00�C�L���������Ӆ&^@7b⠃ˉ�w~-=���|��Ͽ��z��S����qY,��f���W����Y7���Q_�1���[\��HL������\E�ސB���b�Y��v�P,�*�c�{�a�1V�����	r|c�!�ц��v혘�R�蠅	zH��ؤ��p�������H9�iZZ]��]�ꦮ��<f�52��e
���
�~�>Q��;$�uӱNp�W��e�m�/���i�?��`k(����    IEND�B`�
------WebKitFormBoundaryuw0V7pwMnMgyV7Qr
Content-Disposition: form-data; name="email_address"

123
------WebKitFormBoundaryuw0V7pwMnMgyV7Qr
Content-Disposition: form-data; name="Submit"

Submit Form
------WebKitFormBoundaryuw0V7pwMnMgyV7Qr--
------WebKitFormBoundaryMU7HPo1VNvr6rTpJ
Content-Disposition: form-data; name="photo"; filename="img.png"
Content-Type: image/png

�PNG

   IHDR         ��   sRGB ���   �eXIfMM *                  J       R(       �i       Z      �     �    �       �       �           ]I
8   	pHYs , ,.
�;  YiTXtXML:com.adobe.xmp     <x:xmpmeta xmlns:x="adobe:ns:meta/" x:xmptk="XMP Core 5.4.0">
   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
      <rdf:Description rdf:about=""
            xmlns:tiff="http://ns.adobe.com/tiff/1.0/">
         <tiff:Orientation>1</tiff:Orientation>
      </rdf:Description>
   </rdf:RDF>
</x:xmpmeta>
L�'Y  IDAT8�ӱN1��v$qd��00�C�L���������Ӆ&^@7b⠃ˉ�w~-=���|��Ͽ��z��S����qY,��f���W����Y7���Q_�1���[\��HL������\E�ސB���b�Y��v�P,�*�c�{�a�1V�����	r|c�!�ц��v혘�R�蠅	zH��ؤ��p�������H9�iZZ]��]�ꦮ��<f�52��e
���
�~�>Q��;$�uӱNp�W��e�m�/���i�?��`k(����    IEND�B`�
------WebKitFormBoundaryMU7HPo1VNvr6rTpJ
Content-Disposition: form-data; name="email_address"

123
------WebKitFormBoundaryMU7HPo1VNvr6rTpJ
Content-Disposition: form-data; name="Submit"

Submit Form
------WebKitFormBoundaryMU7HPo1VNvr6rTpJ--
------WebKitFormBoundaryDQ5FS7AwyiddKZaQ
Content-Disposition: form-data; name="photo"; filename="img.png"
Content-Type: image/png

�PNG

   IHDR         ��   sRGB ���   �eXIfMM *                  J       R(       �i       Z      �     �    �       �       �           ]I
8   	pHYs , ,.
�;  YiTXtXML:com.adobe.xmp     <x:xmpmeta xmlns:x="adobe:ns:meta/" x:xmptk="XMP Core 5.4.0">
   <rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#">
      <rdf:Description rdf:about=""
            xmlns:tiff="http://ns.adobe.com/tiff/1.0/">
         <tiff:Orientation>1</tiff:Orientation>
      </rdf:Description>
   </rdf:RDF>
</x:xmpmeta>
L�'Y  IDAT8�ӱN1��v$qd��00�C�L���������Ӆ&^@7b⠃ˉ�w~-=���|��Ͽ��z��S����qY,��f���W����Y7���Q_�1���[\��HL������\E�ސB���b�Y��v�P,�*�c�{�a�1V�����	r|c�!�ц��v혘�R�蠅	zH��ؤ��p�������H9�iZZ]��]�ꦮ��<f�52��e
���
�~�>Q��;$�uӱNp�W��e�m�/���i�?��`k(����    IEND�B`�
------WebKitFormBoundaryDQ5FS7AwyiddKZaQ
Content-Disposition: form-data; name="email_address"

123
------WebKitFormBoundaryDQ5FS7AwyiddKZaQ
Content-Disposition: form-data; name="Submit"

Submit Form
------WebKitFormBoundaryDQ5FS7AwyiddKZaQ--
