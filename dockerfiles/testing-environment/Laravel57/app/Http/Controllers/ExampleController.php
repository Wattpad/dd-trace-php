<?php

namespace App\Http\Controllers;

use Illuminate\Routing\Controller as BaseController;
use Illuminate\Support\Facades\DB;

class ExampleController extends BaseController
{
    public function example()
    {
        error_log('This is the example action');
        $this->pdoQueries();
        $this->curlRequest();
        return "Hi from Laravel 5.7 app!\n";
    }

    private function pdoQueries()
    {
        $pdo = DB::connection()->getPdo();
        $stm = $pdo->query("show tables");
        error_log('found ' . print_r($stm->fetchAll(), 1));
    }

    private function curlRequest()
    {
        $url = (\getenv('DD_AGENT_HOST') ?: 'localhost' ) . ':8126/not-existing';
        $ch = \curl_init($url);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT_MS, 1);
        curl_setopt($ch, CURLOPT_TIMEOUT_MS, 1);
        \curl_exec($ch);
    }
}