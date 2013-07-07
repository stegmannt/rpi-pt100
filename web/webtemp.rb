#!/usr/bin/ruby -w
# encoding: utf-8

require 'sinatra'

set :bind, '192.168.178.51'
set :port, 8080

get '/temp' do
  file = File.new("temperature.txt")
  temp = file.gets
  file.close
  return "Current temperature: #{temp}°C"
end 
