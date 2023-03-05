<html>
  <head>
    <title>Webserv - File Upload</title>
  </head>
  <body>

    <?php
      $_targetdir = "uploads";
      $_targetfile = $_targetdir . "/" . basename($_FILES["fileToUpload"]["name"]);
      $uploadOk = 1;
      $imageFileType = strtolower(pathinfo($_targetfile,PATHINFO_EXTENSION));

      // Check if image file is a actual image or fake image
      if(isset($_POST["submit"])) {
        $check = getimagesize($_FILES["fileToUpload"]["tmp_name"]);
        if($check !== false) {
          $uploadOk = 1;
        } else {
          echo "File is not an image.\n";
          $uploadOk = 0;
        }
      }

      // Check if file already exists
      if (file_exists($_targetfile)) {
        echo "The file already exists :).\n";
        $uploadOk = 0;
      }

      // Check file size
      if ($_FILES["fileToUpload"]["size"] > 500000) {
        echo "Sorry, your file is too large.\n";
        $uploadOk = 0;
      }

      // Allow certain file formats
      if($imageFileType != "jpg" && $imageFileType != "png" && $imageFileType != "jpeg"
      && $imageFileType != "gif" ) {
        echo "Sorry, only JPG, JPEG, PNG & GIF files are allowed.\n";
        $uploadOk = 0;
      }

      // Check if $uploadOk is set to 0 by an error
      if ($uploadOk == 1) {
        if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $_targetfile)) {
          echo "<p>The file ". htmlspecialchars( basename( $_FILES["fileToUpload"]["name"])). " has been uploaded.</p>";
        } else {
          echo "Sorry, there was an error uploading your file.\n";
        }
      }
    ?>

  </body>
</html>