https://docs.ovh.com/gb/en/customer/first-steps-with-ovh-api/

 OVH API Bash client
================

A bash client for OVH API (https://api.ovh.com/)

Initialize
----------

### Retrieve dependency

First in order to retrieve needed dependency, run:

```
make
```

### Create an OVH API Application

In order to create a new OVH API application, run:

```
./ovh-api-bash-client.sh --initApp
```

### Create a Consumer Key

In order to create a new consumer key, run:
```
./ovh-api-bash-client.sh --init
```

Options
-------

### Show help

```
./ovh-api-bash-client.sh --help
```

Possible arguments are:

```
  --url <url>             : the API URL to call, for example /domains (default is /me)
  --method <method>       : the HTTP method to use, for example POST (default is GET)
  --data <JSON data>      : the data body to send with the request
  --target <target>       : the target API [CA|EU|US] (default is EU)
  --init                  : to initialize the consumer key, and manage custom access rules file
  --initApp               : to initialize the API application
  --list-profile          : list available profiles in ~/.ovh-api-bash-client/profile directory
  --profile <profile>
            * default : from ~/.ovh-api-bash-client/profile directory
            * <dir>   : from ~/.ovh-api-bash-client/profile/<dir> directory
```

Usage
-----

### Just some examples:

To make a basic call on GET `/me` just run:

```
./ovh-api-bash-client.sh
```

To retrieve your domain list, run:

```
./ovh-api-bash-client.sh --url "/domain"
```

To activate the monitoring on your dedicated server, run:

```
./ovh-api-bash-client.sh --method PUT --url "/dedicated/server/ns00000.ovh.net" --data '{"monitoring": true}'
```

To create a Consumer key for different account or usage (profile is created if missing)

```
./ovh-api-bash-client.sh --profile demo1 --init
./ovh-api-bash-client.sh --profile demo2 --init
```

Embedded lib for external scripts
---------------------------------

See **contrib/** directory
