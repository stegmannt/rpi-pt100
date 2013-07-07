#!/usr/bin/ruby -w
# encoding: utf-8

require 'sinatra'

set :bind, '192.168.178.51'
set :port, 8080

get '/temp' do
  file = File.new("temperature.txt")
  temp = file.gets.to_i
  mtime = file.stat.mtime
  file.close
  return "Temperature: #{temp}°C<br/>Modification time: #{mtime}"
end 
