<?php

class User
{
    private $name;

    public function __construct($name)
    {
        $this->name = $name;
    }

    public function saludar()
    {
        echo("Hola $this->name");
    }
}

$user = new User("Mundo");
$user->saludar();

